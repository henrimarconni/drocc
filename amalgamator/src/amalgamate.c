#include "amalgamate.h"
#include "cparser_utils.h"
#include "scanner.h"
#include "span.h"
#include "stringbuilder.h"
#include "utils.h"
#include "vmem_arena.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  StringBuilder output;
  IncludeDirVec include_dirs;
  SourceFile file;
  VMEMArena* arena;
} Amalgamator;

void preprocessor_cmd(Amalgamator* a);

void skip_unwanted(SourceFile* file) {
  int last_id;
  do {
    last_id = file->pos.id;
    Span _span = {};
    skip_c_comments(file, &_span);
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
  Span include_file = parse_cstr(&a->file);
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
    } else
      vmarena_mark_reset(a->arena, mark);
  }
}

void preprocessor_cmd(Amalgamator* a) {
  skip_unwanted(&a->file);
  Span span = span_from_file(&a->file);
  char ch;
  while ((ch = peekch(&a->file)) != EOF) {
    if (span.len > strlen("include"))
      break;
    if (span_str_cmp(span, "include")) {
      skip_space(&a->file);
      if (peekch(&a->file) == '"')
        process_include(a);
      else
        break;
    }
    span.len++;
    nextch(&a->file);
  }
}

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, bstr output, bstr input) {
  Amalgamator a = {};
  ScannerRes res = read_file(&a.file, arena, input);
  assert(res == SE_OK);
  a.arena = arena;
  a.include_dirs = idirs;
  preprocessor_cmd(&a);
  return a.output;
}
