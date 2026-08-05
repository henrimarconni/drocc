#include "amal_diag.h"
#include "amalgamate.h"
#include "core/clexer_utils.h"
#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/stringbuilder.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void skip_unwanted(SrcScanner* scanner) {
  int last_id;
  do {
    last_id = scanner->id;
    skip_c_comments(scanner);
    skip_space(scanner);
  } while (scanner->id != last_id && peekch(scanner) != EOF);
}

static inline void skip_unwanted_str(SrcScanner* scanner) {
  int last_id;
  do {
    last_id = scanner->id;
    skip_c_comments(scanner);
    skip_space(scanner);
    lex_cstr(scanner);
  } while (scanner->id != last_id && peekch(scanner) != EOF);
}

static SrcScanner include(Amalgamator* a, bstr fname) {
  // try to open file directly
  SrcID srcid = sman_open(&a->sman, fname, a->arena);
  if (srcid != INVALID_SRC_ID)
    return scanner_new(&a->sman, srcid);

  // try all directory + file combinations
  size_t fnamelen = strlen(fname);

  for (size_t i = 0; i < a->idirs.n; i++) {
    bstr dir = a->idirs.get[i];
    size_t dirlen = strlen(dir);

    VMEMArenaMark mark = vmarena_mark(a->arena);
    bstr path = vmarena_alloc(a->arena, dirlen + 1 + fnamelen + 1);

    // path = dir + / + fname
    memcpy(path, dir, dirlen);
    path[dirlen] = '/';
    memcpy(path + dirlen + 1, fname, fnamelen);
    path[dirlen + 1 + fnamelen] = '\0';

    srcid = sman_open(&a->sman, path, a->arena);
    printf("%s sheet %d\n", path, srcid);

    if (srcid != INVALID_SRC_ID)
      return scanner_new(&a->sman, srcid);

    // reset arena on failure
    vmarena_mark_reset(a->arena, mark);
  }

  throw_diag(&a->engine, NULL_SPAN, AMAL_ERR_FILE_NOT_FOUND, fname);
}

// TODO: Skip strings while processing
static void append_processed(Amalgamator* a, SrcScanner* scanner) {
  Span span = span_begin(scanner);
  while (peekch(scanner) != EOF) {
    // TODO: #   include support
    if (match_str(scanner, "#include")) {

      // add the text before #include to the output
      span_end(&span, scanner);
      span.len -= strlen("#include");
      append_sv(&a->output, span_sv(&a->sman, span));

      // remove spaces between #include and ""
      Span spaces = span_begin(scanner);
      skip_unwanted(scanner);
      span_end(&spaces, scanner);

      // file name span
      Span fname = span_begin(scanner);
      int res = lex_cstr(scanner);
      span_end(&fname, scanner);

      if (res < -1 || fname.len <= 2) {
        // revert back if lexing string failed
        append_str(&a->output, "#include");
        append_sv(&a->output, span_sv(&a->sman, spaces));
        span = span_begin(scanner);
        continue;
      }

      // remove the surrounding ""
      fname.len -= 2;
      fname.offset += 1;

      char buf[1024];
      dup_span_buf(&a->sman, fname, buf, sizeof(buf));
      SrcScanner included = include(a, buf);
      append_processed(a, &included);

      span = span_begin(scanner);
      continue;
    }
    nextch(scanner);
  }
  skip_unwanted_str(scanner);
  span_end(&span, scanner);
  append_sv(&a->output, span_sv(&a->sman, span));
}

StringBuilder amalgamate(Amalgamator* a, jmp_buf* onerror) {
  a->sman = sman_new();
  a->engine = new_engine(amal_diaginfos, __amal_diaginfos_len, &a->sman, onerror);

  for (size_t i = 0; i < a->files.n; i++) {
    bstr fname = a->files.get[i];
    SrcID srcid = sman_open(&a->sman, fname, a->arena);

    if (srcid == INVALID_SRC_ID)
      throw_diag(&a->engine, NULL_SPAN, AMAL_ERR_FILE_NOT_FOUND, fname);

    SrcScanner scanner = scanner_new(&a->sman, srcid);
    append_processed(a, &scanner);
  }
  append_ch(&a->output, '\0');
  return a->output;
}

void amalgamator_free(Amalgamator* a) {
  vec_destroy(a->output);
  vec_destroy(a->idirs);
  vec_destroy(a->files);
  vmarena_free(a->arena);
  sman_free(&a->sman);
}
