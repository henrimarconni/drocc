#ifndef EXPR_H_
#define EXPR_H_

#include "chucci_parse/parser.h"

#define UNARY_OPS(X)                                                                               \
  X(OP_ADD, "+")                                                                                   \
  X(OP_SUB, "-")                                                                                   \
  X(OP_NOT, "!")                                                                                   \
  X(OP_BIT_NOT, "~")                                                                               \
  X(OP_BIT_AND, "&")                                                                               \
  X(OP_MUL, "*")                                                                                   \
  X(OP_INC, "++")                                                                                  \
  X(OP_DEC, "--")

#define BINARY_OPS(X)                                                                              \
  X(OP_ADD, "+")                                                                                   \
  X(OP_SUB, "-")                                                                                   \
  X(OP_BIT_AND, "&")                                                                               \
  X(OP_SHL_EQ, "<<=")                                                                              \
  X(OP_SHR_EQ, ">>=")                                                                              \
  X(OP_EQ, "==")                                                                                   \
  X(OP_NEQ, "!=")                                                                                  \
  X(OP_LE, "<=")                                                                                   \
  X(OP_GE, ">=")                                                                                   \
  X(OP_AND, "&&")                                                                                  \
  X(OP_OR, "||")                                                                                   \
  X(OP_SHL, "<<")                                                                                  \
  X(OP_SHR, ">>")                                                                                  \
  X(OP_ADD_EQ, "+=")                                                                               \
  X(OP_SUB_EQ, "-=")                                                                               \
  X(OP_MUL_EQ, "*=")                                                                               \
  X(OP_DIV_EQ, "/=")                                                                               \
  X(OP_MOD_EQ, "%=")                                                                               \
  X(OP_AND_EQ, "&=")                                                                               \
  X(OP_OR_EQ, "|=")                                                                                \
  X(OP_XOR_EQ, "^=")                                                                               \
  X(OP_MUL, "*")                                                                                   \
  X(OP_DIV, "/")                                                                                   \
  X(OP_MOD, "%")                                                                                   \
  X(OP_ASSIGN, "=")                                                                                \
  X(OP_LT, "<")                                                                                    \
  X(OP_GT, ">")                                                                                    \
  X(OP_BIT_AND, "&")                                                                               \
  X(OP_BIT_OR, "|")                                                                                \
  X(OP_BIT_XOR, "^")

typedef enum {
  // data: op lhs(Expr) rhs(Expr)
  EXPR_BINOP,
  // data: val(Token)
  EXPR_PRIMARY,
  // data: cond(Expr) iftrue(Expr) else(Expr)
  EXPR_TERNARY,
  // data: op operand(Expr)
  EXPR_UNARY,
  // data: op operand(Expr)
  EXPR_POSTFIX,
  // data: parent(Expr) child(InternID)
  EXPR_DOT_MEMBER_ACCESS,
  // data: parent(Expr) child(InternID)
  EXPR_PTR_MEMBER_ACCESS,
  // data: array(Expr) index(EXPR_PRIMARY)
  EXPR_INDEX,
  // data: num_params(uint8_t) params(Expr[])
  EXPR_CALL,
} ExprKind;

typedef struct Expr {
  ExprKind kind;
  vmptr(void) data; // any of the above
} Expr;

typedef struct {
  TokenKind op;
  Expr lhs;
  Expr rhs;
} ExprBinop;

typedef struct {
  Token val;
} ExprPrimary;

typedef struct {
  Expr cond;
  Expr iftrue;
  Expr iffalse;
} ExprTernary;

typedef struct {
  TokenKind op;
  Expr inner;
} ExprUnary;

typedef struct {
  TokenKind op;
  Expr inner;
} ExprPostfix;

typedef struct {
  Expr parent;
  InternID child;
} ExprDotAccess;

typedef struct {
  Expr parent;
  InternID child;
} ExprPtrAccess;

typedef struct {
  Expr expr;
  Token val;
} ExprIndex;

typedef struct {
  InternID func;
  slice(Expr) args;
} ExprFnCall;

Expr parse_expr(Parser* p);
void print_expr(Parser* p, Expr expr);

#endif
