/**
  A simple pratt parser ;-)
*/

#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/expr.h"
#include "chucci_parse/parser.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static int binding_power(TokenKind kind) {
  switch (kind) {
#define X(a, bp, _)                                                                                \
  case a:                                                                                          \
    return bp;
    BINDING_POWERS(X)
#undef X
  default:
    assert(false && "INVALID TOKEN");
  }
}

static Expr* make_expr(Parser* p, ExprKind kind, void* data, size_t size) {
  Expr* expr = vmarena_alloc(p->arena, sizeof(Expr) + size);
  memcpy(expr->data, data, size);
  expr->kind = kind;
  return expr;
}

#define data_size sizeof(TokenKind) + sizeof(Expr*)

static Expr* left_denotation(Parser* p, Expr* left, Token op) { return NULL; }

static Expr* null_denotation(Parser* p, Token token) {
  if (token.kind == TOK_VAL || token.kind == TOK_STR || token.kind == TOK_IDENT)
    return make_expr(p, EXPR_PRIMARY, &token, sizeof(token));

  if (is_unary[token.kind]) {
    Expr* inner = parse_expr(p);

    uint8_t data[data_size] = {0};
    *(TokenKind*)data = token.kind;
    *(Expr**)(data + sizeof(TokenKind)) = inner;

    return make_expr(p, EXPR_UNARY, &data, data_size);
  }

  assert(false && "BAD LEFT_DENOTATION");
}

static Expr* parse_expr_bp(Parser* p, int bp) {
  Token token = ts_next(&p->ts);

  // Handle prefix tokens (numbers, variables, prefix '-' or '!')
  Expr* left = null_denotation(p, token);

  Token peeked = ts_peek(&p->ts);

  while (peeked.kind != TOK_EOF && binding_power(peeked.kind) > bp) {
    Token next_op = ts_next(&p->ts);
    // Handle infix/postfix operators (like +, *, or a function call)
    left = left_denotation(p, left, next_op);
  }

  return left;
}

// +10 to get rid of any underflow issues
Expr* parse_expr(Parser* p) { return parse_expr_bp(p, -INT_MAX + 10); }
