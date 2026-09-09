#ifndef STMT_H_
#define STMT_H_

#include "chucci_parse/expr.h"
#include "core/vmem_arena.h"
#include <stdint.h>

typedef struct Parser Parser;

typedef enum {
  STMT_RETURN
} StmtKind;

typedef struct {
  Expr expr;
} StmtReturn;

typedef struct {
  StmtKind kind;
  vmptr(void) data;
} Stmt;

void print_stmt(Parser* p, Stmt stmt);
Stmt parse_stmt(Parser* p);

#endif
