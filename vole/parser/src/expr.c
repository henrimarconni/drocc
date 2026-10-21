#include "core/diagnostics.h"
#include "lexer/token.h"
#include "lexer/token_stream.h"
#include "parser/expr.h"
#include "parser/parser.h"
#include "parser/type.h"
#include "volecore/diag.h"
#include <stdint.h>

Expr parse_expr(Parser* p) {
  Expr expr = {0};

  Token tok = ts_next(&p->ts);

  if (tok.kind == TOK_VAL) {
    expr.kind = EXPR_NUM;
    expr.type.pt = PT_INT;
    expr.type.kind = TK_PRIMITIVE;
    return expr;
  }

  throw_diag(p->engine, tok.span, CC_PRS_EXPECTED_EXPR);
}
