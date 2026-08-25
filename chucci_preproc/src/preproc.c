#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_preproc/preproc.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"

static InternID preproc_ids[_preproc_cmd_count] = {0};

static PreprocCMD tok2cmd(Preprocessor* pp, Token token) {

  if (preproc_ids[0] == 0) {
#define X(kind, str) preproc_ids[kind] = intern(strview(str), pp->interner);
    PREPROC_CMDS(X)
#undef X
  }

  if (token.kind != TOK_IDENT)
    __builtin_unreachable();
#define X(kind, str)                                                                               \
  if (preproc_ids[kind] == token.ident) {                                                          \
    return kind;                                                                                   \
  }

  __builtin_unreachable();
}

TokenStream preproc_new(TokenStream ts, SourceManager* sman, StringInterner* interner) {
  VMEMArena* arena = vmarena_new(1024 * 1024);
  Preprocessor* preproc = vmarena_calloc(arena, sizeof(VMEMArena));
  preproc->arena = arena;
  preproc->interner = interner;
  preproc->sman = sman;

  vec_push(preproc->stack, ts);
  TokenStream pp_ts = ts_from_func(preproc, preproc_next, preproc_peek, preproc_free);
  return pp_ts;
}

static Token preproc_stmt(Preprocessor* pp) {
  Token token = tstack_next(&pp->stack);

  switch (tok2cmd(pp, token)) { case PP_INCLUDE: }

  return token;
}

Token preproc_next(void* ctx) {
  Preprocessor* pp = ctx;

  Token token = tstack_next(&pp->stack);

  // Found preprocessor statement
  if (token.kind == OP_PREPROCESS) {
    return preproc_stmt(pp).
  }

  // else return the token
  return token;
}
