#ifndef EXPR_H_
#define EXPR_H_

#include "parser/parser.h"
#include "parser/type.h"
#include <stdint.h>

typedef enum { EXPR_NUM } ExprKind;

typedef struct {
  ExprKind kind;
  VoleType type;
  union {
    uint32_t num;
  };
} Expr;

Expr parse_expr(Parser* p);

#endif
