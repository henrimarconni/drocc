#ifndef __TOKEN_STREAM_H
#define __TOKEN_STREAM_H

#include "chucci_lex/token.h"
#include "core/diagnostics.h"
#include "core/vec.h"

typedef Token (*TSNextFn)(void*);
typedef Token (*TSPeekFn)(void*);
typedef void (*TSFreeCtxFn)(void*);

typedef enum TokenStreamKind {
  TS_VEC,
  /// Function based, like lex_next_token or pp_next_token
  TS_FUNC,
  /// Single token
  TS_SINGLE,
} TokenStreamKind;

typedef struct {
  void* ctx;
  TSNextFn next;
  TSPeekFn peek;
  TSFreeCtxFn free;
} TSFuncStream;

typedef struct {
  TokenVec vec;
  size_t pos;
} TSVecStream;

typedef struct TokenStream {
  TokenStreamKind kind;
  bool is_consumed;
  union {
    TSVecStream vstream;
    TSFuncStream fstream;
    Token single;
  };
} TokenStream;

typedef vec(TokenStream) TokenStreamStack;

TokenStream ts_from_vec(TokenVec vec);
TokenStream ts_from_token(Token token);
TokenStream ts_from_func(void* ctx, TSNextFn next, TSPeekFn peek, TSFreeCtxFn free);
Token ts_next(TokenStream* ts);
Token ts_peek(TokenStream* ts);
Token ts_expect(TokenStream* ts, TokenKind kind, DiagEngine* engine);
void ts_free(TokenStream* ts);

Token tstack_expect(TokenStreamStack* stack, TokenKind kind, DiagEngine* engine);
Token tstack_peek(TokenStreamStack* stack);
Token tstack_next(TokenStreamStack* stack);
void tstack_free(TokenStreamStack* ts);

#endif
