#include "amal_diag.h"
#include "amalgamate.h"
#include "clexer_utils.h"
#include "diagnostics.h"
#include "scanner.h"
#include "span.h"
#include "stringbuilder.h"
#include "utils.h"
#include "vmem_arena.h"
#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  StringBuilder output;
  IncludeDirVec include_dirs;
  SourceFile file;
  VMEMArena* arena;
  DiagEngine engine;
} Amalgamator;

void preprocessor_cmd(Amalgamator* a);

void skip_unwanted(SourceFile* file) {
  int last_id;
  do {
    last_id = file->pos.id;
    skip_c_comments(file);
    skip_space(file);
  } while (file->pos.id != last_id && peekch(file) != EOF);
}

void process_file(Amalgamator* a) {
  char ch;
  while ((ch = peekch(&a->file)) != EOF) {
    skip_unwanted(&a->file);
    if (ch == '#')
      preprocessor_cmd(a);
  }
}

void process_include(Amalgamator* a) {
  Span include_file = span_begin(&a->file);
  if (lex_cstr(&a->file) != CLEX_OK) {
    span_end(&include_file);
    throw_diag(&a->engine, include_file, AMAL_ERR_INVALID_STR, include_file);
  }
  span_end(&include_file);
  shrink_span(&include_file);
  advance_span(&include_file);
  VMEMArenaMark mark = vmarena_mark(a->arena);
  for (size_t i = 0; i < a->include_dirs.n; i++) {
    bstr included_dir = a->include_dirs.get[i];
    size_t included_dir_len = strlen(included_dir);
    // path = included_dir + '/' + include_file + '\0'
    size_t pathlen = included_dir_len + 1 + include_file.len + 1;
    bstr path = vmarena_alloc(a->arena, pathlen);

    memcpy(path, included_dir, included_dir_len);
    path[included_dir_len] = '/';
    memcpy(path + included_dir_len + 1, include_file.str, include_file.len);
    path[pathlen - 1] = '\0';

    if (file_exists(path)) {
      SourceFile file;
      read_file(&file, a->arena, path);
      printf("%s", file.contents);
    } else
      vmarena_mark_reset(a->arena, mark);
  }
}

void preprocessor_cmd(Amalgamator* a) {
  skip_unwanted(&a->file);
  char ch = peekch(&a->file);
  while (peekch(&a->file) != EOF) {
    skip_unwanted(&a->file);

    if (peekch(&a->file) != '#') {
      nextch(&a->file);
      continue;
    }

    nextch(&a->file); // '#'

    if (match_str(&a->file, "include")) {
      skip_unwanted(&a->file);
      if (peekch(&a->file) == '"')
        process_include(a);
      else
        continue;
    } else
      continue;
  }
}

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, bstr output, bstr input,
                         jmp_buf* onerror) {
  Amalgamator a = {};
  a.engine = new_engine(amal_diaginfos, __amal_diaginfos_len, onerror);
  ScannerRes res = read_file(&a.file, arena, input);
  assert(res == SE_OK);
  a.arena = arena;
  a.include_dirs = idirs;
  preprocessor_cmd(&a);
  return a.output;
}
