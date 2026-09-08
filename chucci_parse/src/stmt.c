#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/expr.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/stmt.h"
#include <assert.h>
#include <stdio.h>

static Stmt* make_stmt(Parser* p, StmtKind kind, void* data, size_t size) {
  Stmt* stmt = vmarena_alloc(p->arena, sizeof(Stmt) + size);
  memcpy(stmt->data, data, size);
  stmt->kind = kind;
  return stmt;
}

Stmt* parse_stmt(Parser* p) {
  // currently only return is supported

  Token token = ts_next(&p->ts);
  if (token.kind == KW_RETURN) {
    Expr* expr = parse_expr(p);
    uint8_t data[sizeof(Expr*)] = {0};
    *(Expr**)data = expr;
    Token semi = ts_next(&p->ts);
    assert(semi.kind == SEP_SEMI);

    return make_stmt(p, STMT_RETURN, data, sizeof(Expr*));
  }

  assert(false);
  return NULL;
}

void print_stmt(Parser* p, Stmt* stmt) {
  switch (stmt->kind) {
  case STMT_RETURN:
    printf("return ");
    print_expr(p, *(Expr**)stmt->data);
    puts(";");
    break;
  }
}
