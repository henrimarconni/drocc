#include "core/diagnostics.h"
#include "lexer/token.h"
#include "lexer/token_stream.h"
#include "parser/expr.h"
#include "parser/stmt.h"
#include "volecore/diag.h"

Stmt parse_stmt(Parser* p) {
  Token tok = ts_next(&p->ts);
  switch (tok.kind) {
  case KW_RETURN:
    Expr expr = parse_expr(p);
    Stmt stmt = {0};
    stmt.kind = STMT_RETURN;
    stmt.ret_expr = expr;
    ts_expect(&p->ts, SEP_SEMI, p->engine);
    return stmt;
  default:
    throw_diag(p->engine, tok.span, CC_PRS_EXPECTED_STMT);
  }
}
