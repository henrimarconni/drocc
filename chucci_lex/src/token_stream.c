#include "chucci_lex/cc_diag.h"
#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "core/diagnostics.h"
#include "core/vec.h"
#include <assert.h>

TokenStream ts_from_vec(TokenVec vec) {
  TokenStream ts = {0};
  ts.kind = TS_VEC;
  ts.vstream.vec = vec;
  return ts;
}

TokenStream ts_from_token(Token token) {
  TokenStream ts = {0};
  ts.kind = TS_SINGLE;
  ts.single = token;
  return ts;
}

TokenStream ts_from_func(void* ctx, TSNextFn next, TSPeekFn peek, TSFreeCtxFn free) {
  TokenStream ts = {0};
  ts.kind = TS_FUNC;
  ts.fstream.next = next;
  ts.fstream.peek = peek;
  ts.fstream.free = free;
  ts.fstream.ctx = ctx;
  return ts;
}

Token ts_next(TokenStream* ts) {
  if (ts->is_consumed)
    return EOF_TOKEN;

  switch (ts->kind) {
  case TS_SINGLE: {
    ts->is_consumed = true;
    return ts->single;
  }

  case TS_VEC: {
    if (ts->vstream.pos >= ts->vstream.vec.n) {
      ts->is_consumed = true;
      return EOF_TOKEN;
    }
    Token token = ts->vstream.vec.get[ts->vstream.pos++];
    if (ts->vstream.pos >= ts->vstream.vec.n)
      ts->is_consumed = true;
    return token;
  }

  case TS_FUNC:
    return ts->fstream.next(ts->fstream.ctx);
  }
}

Token ts_peek(TokenStream* ts) {
  switch (ts->kind) {
  case TS_SINGLE:
    ts->is_consumed = true;

  case TS_VEC:
    if (ts->vstream.pos >= ts->vstream.vec.n) {
      ts->is_consumed = true;
      return EOF_TOKEN;
    }
    return ts->vstream.vec.get[ts->vstream.pos];

  case TS_FUNC:
    return ts->fstream.peek(ts->fstream.ctx);
  }
}

Token ts_expect(TokenStream* ts, TokenKind kind, DiagEngine* engine) {
  Token token = ts_next(ts);
  if (token.kind != kind) {
    throw_diag(
        engine, token.span, CC_ERR_UNEXPECTED_TOKEN, tok_to_str[kind], tok_to_str[token.kind]);
  }

  return token;
}

void ts_free(TokenStream* ts) {
  switch (ts->kind) {
  case TS_VEC:
    vec_destroy(ts->vstream.vec);
    break;
  case TS_FUNC:
    ts->fstream.free(ts->fstream.ctx);
    break;
  case TS_SINGLE:
    break;
  }
}

Token tstack_expect(TokenStreamStack* stack, TokenKind kind, DiagEngine* engine) {
  Token token = tstack_next(stack);
  if (token.kind != kind) {
    throw_diag(
        engine, token.span, CC_ERR_UNEXPECTED_TOKEN, tok_to_str[kind], tok_to_str[token.kind]);
  }

  return token;
}

Token tstack_peek(TokenStreamStack* stack) {
  if (stack->n == 0)
    return EOF_TOKEN;

  TokenStream* stream = &stack->get[stack->n - 1];
  return ts_peek(stream);
}

Token tstack_next(TokenStreamStack* stack) {
  // Keep looping until we find a real token or the whole stack is empty
  while (stack->n > 0) {
    TokenStream* stream = &stack->get[stack->n - 1];

    if (stream->is_consumed) {
      ts_free(stream);
      vec_pop(*stack);
      continue;
    }

    Token token = ts_next(stream);

    if (stream->is_consumed) {
      ts_free(stream);
      vec_pop(*stack);
    }

    // If the token is valid, return it.
    // If it's a EOF_TOKEN, loop and try the next stream
    if (token.kind != TOK_EOF) {
      return token;
    }
  }

  return EOF_TOKEN;
}

void tstack_free(TokenStreamStack* ts) {
  for (size_t i = 0; i < ts->n; i++)
    ts_free(&ts->get[i]);

  vec_destroy(*ts);
}
