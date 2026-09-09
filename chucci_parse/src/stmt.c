#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/expr.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/stmt.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static Stmt make_stmt(Parser* p, StmtKind kind, void* data, size_t size) {
  Stmt stmt = {0};
  stmt.kind = kind;
  stmt.data = vmarena_vmalloc(p->parena, size);
  void* ptr = vmderef(p->parena, stmt.data);
  memcpy(ptr, data, size);

  return stmt;
}

Stmt parse_stmt(Parser* p) {
  // currently only return is supported

  Token token = ts_next(&p->ts);
  if (token.kind == KW_RETURN) {
    Expr expr = parse_expr(p);
    StmtReturn r = {0};
    r.expr = expr;
    Token semi = ts_next(&p->ts);
    assert(semi.kind == SEP_SEMI);

    return make_stmt(p, STMT_RETURN, &r, sizeof(r));
  }

  assert(false);
  return (Stmt){0};
}

void print_stmt(Parser* p, Stmt stmt) {
  switch (stmt.kind) {
  case STMT_RETURN:
    printf("return ");
    StmtReturn* ret = vmderef(p->parena, stmt.data);
    print_expr(p, ret->expr);
    puts(";");
    break;
  }
}
