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

#define BINDING_POWERS(X)                                                                          \
  /* Token Enum     | Power | String representation */                                             \
  X(OP_SHL_EQ, 1, "<<=")                                                                           \
  X(OP_SHR_EQ, 1, ">>=")                                                                           \
  X(OP_ELLIPSIS, 0, "...")                                                                         \
  X(OP_INC, 13, "++")                                                                              \
  X(OP_DEC, 13, "--")                                                                              \
  X(OP_EQ, 8, "==")                                                                                \
  X(OP_NEQ, 8, "!=")                                                                               \
  X(OP_LE, 9, "<=")                                                                                \
  X(OP_GE, 9, ">=")                                                                                \
  X(OP_AND, 4, "&&")                                                                               \
  X(OP_OR, 3, "||")                                                                                \
  X(OP_SHL, 10, "<<")                                                                              \
  X(OP_SHR, 10, ">>")                                                                              \
  X(OP_TOKEN_PASTE, 14, "##")                                                                      \
  X(OP_ADD_EQ, 1, "+=")                                                                            \
  X(OP_SUB_EQ, 1, "-=")                                                                            \
  X(OP_MUL_EQ, 1, "*=")                                                                            \
  X(OP_DIV_EQ, 1, "/=")                                                                            \
  X(OP_MOD_EQ, 1, "%=")                                                                            \
  X(OP_AND_EQ, 1, "&=")                                                                            \
  X(OP_OR_EQ, 1, "|=")                                                                             \
  X(OP_XOR_EQ, 1, "^=")                                                                            \
  X(OP_ARROW, 14, "->")                                                                            \
  X(OP_ADD, 11, "+")                                                                               \
  X(OP_SUB, 11, "-")                                                                               \
  X(OP_MUL, 12, "*")                                                                               \
  X(OP_DIV, 12, "/")                                                                               \
  X(OP_MOD, 12, "%")                                                                               \
  X(OP_ASSIGN, 1, "=")                                                                             \
  X(OP_LT, 9, "<")                                                                                 \
  X(OP_GT, 9, ">")                                                                                 \
  X(OP_NOT, 13, "!")                                                                               \
  X(OP_BIT_AND, 7, "&")                                                                            \
  X(OP_BIT_OR, 5, "|")                                                                             \
  X(OP_BIT_XOR, 6, "^")                                                                            \
  X(OP_BIT_NOT, 13, "~")                                                                           \
  X(OP_QUESTION, 2, "?")                                                                           \
  X(OP_COLON, 2, ":")                                                                              \
  X(OP_PREPROCESS, 0, "#")                                                                         \
  X(OP_DOT, 14, ".")                                                                               \
  X(SEP_LPAREN, 0, "(")                                                                            \
  X(SEP_RPAREN, 0, ")")                                                                            \
  X(SEP_LCURLY, 0, "{")                                                                            \
  /* Note: Separators and layout have 0 binding power */                                           \
  X(SEP_RCURLY, 0, "}")                                                                            \
  X(SEP_LSQ, 0, "[")                                                                               \
  X(SEP_RSQ, 0, "]")                                                                               \
  X(SEP_COMMA, 0, ",")                                                                             \
  X(SEP_NEWLINE, 0, "\n")                                                                          \
  X(SEP_SEMI, 0, ";")

Expr* parse_expr(Parser* p);

#endif
