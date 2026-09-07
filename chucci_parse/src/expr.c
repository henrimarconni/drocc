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

static Expr* parse_expr_bp(Parser* p, int bp);

void print_expr(Parser* p, Expr* expr) {
  putchar('(');
  switch (expr->kind) {
  case EXPR_PRIMARY:
    print_token(p->sman, ((Token*)expr->data));
    break;
  case EXPR_UNARY:
    printf("%s", tok_to_str[*(TokenKind*)expr->data]);
    Expr* inner = *(Expr**)(expr->data + sizeof(TokenKind));
    print_expr(p, inner);
    break;
  case EXPR_BINOP:
    Expr* lhs = *(Expr**)(expr->data + sizeof(TokenKind));
    print_expr(p, lhs);
    printf(" %s ", tok_to_str[*(TokenKind*)expr->data]);
    Expr* rhs = *(Expr**)(expr->data + sizeof(TokenKind) + sizeof(Expr*));
    print_expr(p, rhs);
    break;
  default:
    printf("NOT IMPLEMENTED\n");
    break;
  }
  putchar(')');
}

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

static Expr* left_denotation(Parser* p, Expr* left, Token op) {
#define data_size sizeof(TokenKind) + sizeof(Expr*) * 2
  if (is_binary[op.kind]) {
    Expr* rhs = parse_expr_bp(p, binding_power(op.kind));

    uint8_t data[data_size] = {0};
    *(TokenKind*)data = op.kind;
    *(Expr**)(data + sizeof(TokenKind)) = left;
    *(Expr**)(data + sizeof(TokenKind) + sizeof(Expr*)) = rhs;

    return make_expr(p, EXPR_BINOP, data, data_size);
  }
  assert(false && "NOT IMPLEMENTED");
#undef data_size
}

static Expr* null_denotation(Parser* p, Token token) {
#define data_size sizeof(TokenKind) + sizeof(Expr*)
  if (token.kind == TOK_VAL || token.kind == TOK_STR || token.kind == TOK_IDENT)
    return make_expr(p, EXPR_PRIMARY, &token, sizeof(token));

  if (is_unary[token.kind]) {
    Expr* inner = parse_expr_bp(p, binding_power(token.kind));

    uint8_t data[data_size] = {0};
    *(TokenKind*)data = token.kind;
    *(Expr**)(data + sizeof(TokenKind)) = inner;

    return make_expr(p, EXPR_UNARY, &data, data_size);
  }

  if (token.kind == SEP_LPAREN) {
    Expr* expr = parse_expr_bp(p, binding_power(token.kind));
    // TODO: ts_expect
    Token next = ts_next(&p->ts);
    assert(next.kind == SEP_RPAREN);
    return expr;
  }

  assert(false && "BAD LEFT_DENOTATION");
#undef data_size
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
    peeked = ts_peek(&p->ts);
  }

  return left;
}

// +10 to get rid of any underflow issues
Expr* parse_expr(Parser* p) { return parse_expr_bp(p, -INT_MAX + 10); }
