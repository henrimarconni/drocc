#include "amal_diag.h"
#include "amalgamate.h"
#include "clexer_utils.h"
#include "diagnostics.h"
#include "scanner.h"
#include "span.h"
#include "stringbuilder.h"
#include "vec.h"
#include "vmem_arena.h"
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  StringBuilder output;
  IncludeDirVec idirs;
  vec(SourceFile) cache;
  VMEMArena* arena;
  DiagEngine engine;
} Amalgamator;

void skip_unwanted(SourceFile* file) {
  int last_id;
  do {
    last_id = file->pos.id;
    skip_c_comments(file);
    skip_space(file);
  } while (file->pos.id != last_id && peekch(file) != EOF);
}

void skip_unwanted_str(SourceFile* file) {
  int last_id;
  do {
    last_id = file->pos.id;
    skip_c_comments(file);
    skip_space(file);
    lex_cstr(file);
  } while (file->pos.id != last_id && peekch(file) != EOF);
}

bool check_cached(SourceFile* file, Amalgamator* a, bstr fname) {
  for (size_t i = 0; i < a->cache.n; i++) {
    *file = a->cache.get[i];
    if (strcmp(file->name, fname) == 0)
      return true;
  }
  *file = (SourceFile){0};
  return false;
}

SourceFile include(Amalgamator* a, bstr fname) {
  SourceFile file = {};
  if (check_cached(&file, a, fname)) {
    return file;
  }
  ScannerRes res = read_file(&file, a->arena, fname);
  if (res == SE_OK) {
    vec_push(a->cache, file);
    return file;
  }
  for (size_t i = 0; i < a->idirs.n; i++) {
    bstr dir = a->idirs.get[i];
    size_t dirlen = strlen(dir);
    size_t fnamelen = strlen(fname);
    // dir + '/' + fname + '\0'
    bstr path = vmarena_alloc(a->arena, dirlen + 1 + fnamelen + 1);
    memcpy(path, dir, dirlen);
    path[dirlen] = '/';
    memcpy(path + dirlen + 1, fname, fnamelen);
    path[dirlen + fnamelen + 1] = '\0';

    if (check_cached(&file, a, path)) {
      return file;
    }
    ScannerRes res = read_file(&file, a->arena, path);
    if (res == SE_OK) {
      vec_push(a->cache, file);
      return file;
    }
  }
  throw_diag(&a->engine, NULL_SPAN, AMAL_ERR_FILE_NOT_FOUND, fname);
}

// TODO: Skip strings while processing
void append_processed(Amalgamator* a, SourceFile* file) {
  Span span = span_begin(file);
  while (peekch(file) != EOF) {
    // TODO: #   include support
    if (match_str(file, "#include")) {
      span_end(&span);
      span.len -= strlen("#include");
      append_span(&a->output, span);

      Span spaces = span_begin(file);
      skip_unwanted(file);
      span_end(&spaces);

      Span fname = span_begin(file);
      CLexerRes res = lex_cstr(file);
      span_end(&fname);

      if (res != CLEX_OK) {
        append_str(&a->output, "#include");
        append_span(&a->output, spaces);
        span = span_begin(file);
        continue;
      }

      shrink_span(&fname);
      advance_span(&fname);
      char buf[1024];
      dup_span_buf(fname, buf);
      SourceFile included = include(a, buf);
      append_processed(a, &included);

      span = span_begin(file);
      continue;
    }
    nextch(file);
  }
  skip_unwanted_str(file);
  span_end(&span);
  append_span(&a->output, span);
}

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, InputFIleVec input_files,
                         jmp_buf* onerror) {
  Amalgamator a = {};
  a.arena = arena;
  a.idirs = idirs;
  a.engine = new_engine(amal_diaginfos, __amal_diaginfos_len, onerror);
  for (size_t i = 0; i < input_files.n; i++) {
    bstr fname = input_files.get[i];
    SourceFile file = {};
    ScannerRes res = read_file(&file, arena, fname);
    if (res != SE_OK)
      throw_diag(&a.engine, NULL_SPAN, AMAL_ERR_FILE_NOT_FOUND, fname);
    append_processed(&a, &file);
  }
  append_ch(&a.output, '\0');
  vec_destroy(a.cache);
  return a.output;
}
