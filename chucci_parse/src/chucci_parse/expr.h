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

Expr* parse_expr(Parser* p);
void print_expr(Parser* p, Expr* expr);

#endif
