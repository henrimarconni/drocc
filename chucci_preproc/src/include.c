#include "chucci_diag/cc_diag.h"
#include "chucci_lex/lexer.h"
#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_preproc/include.h"
#include "chucci_preproc/preproc.h"
#include "core/diagnostics.h"
#include "core/srcman.h"
#include "core/stringdef.h"

static bool
include_file(Preprocessor* pp, TokenStream* out_ts, StringView file, PPSearchPaths paths) {
  SrcScanner scanner;
  VMEMArenaMark mark = vmarena_mark(pp->arena);

  // copy the path
  bstr path = vmarena_alloc(pp->arena, file.len + 1);
  memcpy(path, file.str, file.len);
  path[file.len] = '\0';

  if (sman_open(&scanner, pp->sman, path)) {
    *out_ts = lexer_new(pp->sman, scanner, pp->interner, pp->engine.onerror);
    return true;
  }

  // reset on failure
  vmarena_mark_reset(pp->arena, mark);

  // try all directory + file combinations
  for (size_t i = 0; i < paths.n; i++) {
    bstr dir = paths.get[i];
    size_t dirlen = strlen(dir);

    mark = vmarena_mark(pp->arena);
    path = vmarena_alloc(pp->arena, dirlen + 1 + file.len + 1);

    // path = dir + / + file
    memcpy(path, dir, dirlen);
    path[dirlen] = '/';
    memcpy(path + dirlen + 1, file.str, file.len);
    path[dirlen + 1 + file.len] = '\0';

    if (sman_open(&scanner, pp->sman, path)) {
      *out_ts = lexer_new(pp->sman, scanner, pp->interner, pp->engine.onerror);
      return true;
    }

    // reset arena on failure
    vmarena_mark_reset(pp->arena, mark);
  }

  return false;
}

TokenStream preproc_parse_include(Preprocessor* pp) {
  Token token = tstack_next(&pp->stack);
  TokenStream ts = {0};
  SMSpanInfo info = sman_info(pp->sman, token.span);

  if (info.sv.len < 2)
    return ts;

  // skip <> or ""
  info.sv.len -= 2;
  info.sv.str += 1;

  if (token.kind == TOK_STR) {
    if (include_file(pp, &ts, info.sv, pp->search) ||
        include_file(pp, &ts, info.sv, pp->sys_search))
      return ts;
    else
      throw_diag(&pp->engine, token.span, CC_PP_INCLUDED_FILE_NOT_FOUND, info.sv);

  } else if (token.kind == TOK_ANGLE) {
    if (include_file(pp, &ts, info.sv, pp->sys_search) ||
        include_file(pp, &ts, info.sv, pp->search))
      return ts;
    else
      throw_diag(&pp->engine, token.span, CC_PP_INCLUDED_FILE_NOT_FOUND, info.sv);
  }

  return ts;
}
