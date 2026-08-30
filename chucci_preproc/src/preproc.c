#include "chucci_diag/cc_diag.h"
#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_preproc/include.h"
#include "chucci_preproc/preproc.h"
#include "core/diagnostics.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static InternID preproc_ids[_preproc_cmd_count] = {0};

static PreprocCMD tok2cmd(Preprocessor* pp, Token token) {
  if (token.kind != TOK_IDENT)
    __builtin_unreachable();

#define X(kind, str)                                                                               \
  if (preproc_ids[kind] == token.ident) {                                                          \
    return kind;                                                                                   \
  }
  PREPROC_CMDS(X)
#undef X

  SMSpanInfo info = sman_info(pp->sman, token.span);
  printf("%s(%.*s) ", tok_to_str[token.kind], info.sv.len, info.sv.str);
  fflush(stdout);
  __builtin_unreachable();
}

TokenStream preproc_new(
    TokenStream ts,
    SourceManager* sman,
    StringInterner* interner,
    PPSearchPaths sys_search,
    PPSearchPaths search,
    VMEMArena* arena,
    jmp_buf* onerror) {
  Preprocessor* preproc = vmarena_calloc(arena, sizeof(Preprocessor));
  preproc->arena = arena;
  preproc->interner = interner;
  preproc->sman = sman;
  preproc->sys_search = sys_search;
  preproc->search = search;
  preproc->is_peeked = false;
  preproc->engine = new_engine(cc_diaginfos, _cc_diaginfos_len, sman, onerror);

  if (preproc_ids[0] == 0) {
#define X(kind, str) preproc_ids[kind] = intern(strview(str), preproc->interner);
    PREPROC_CMDS(X)
#undef X
  }

  vec_push(preproc->stack, ts);
  TokenStream pp_ts = ts_from_func(preproc, preproc_next, preproc_peek, preproc_free);
  return pp_ts;
}

static Token preproc_stmt(Preprocessor* pp) {
  Token token = tstack_next(&pp->stack);

  switch (tok2cmd(pp, token)) {
  case PP_INCLUDE:
    TokenStream ts = preproc_parse_include(pp);
    vec_push(pp->stack, ts);
    token = tstack_next(&pp->stack);
    break;
  default:
    assert(false && "NOT IMPLEMENTED");
  }

  return token;
}

Token preproc_next(void* ctx) {
  Preprocessor* pp = ctx;
  if (pp->is_peeked) {
    pp->is_peeked = false;
    return pp->peeked;
  }

  Token token = tstack_next(&pp->stack);

  // Found preprocessor statement
  if (token.kind == OP_PREPROCESS) {
    return preproc_stmt(pp);
  }

  // else return the token
  return token;
}

Token preproc_peek(void* ctx) {
  Preprocessor* pp = ctx;
  if (!pp->is_peeked) {
    pp->peeked = preproc_next(pp);
    pp->is_peeked = true;
  }

  return pp->peeked;
}

void preproc_free(void** preproc) { (void)preproc; }
