#ifndef STMT_H_
#define STMT_H_

#include "parser/expr.h"

typedef enum {
  STMT_RETURN
} StmtKind;

typedef struct Stmt {
  StmtKind kind;
  union {
    Expr ret_expr;
  };
} Stmt;

Stmt parse_stmt(Parser* p);

#endif
