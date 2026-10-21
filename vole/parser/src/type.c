#include "core/diagnostics.h"
#include "lexer/token.h"
#include "lexer/token_stream.h"
#include "parser/parser.h"
#include "parser/type.h"
#include "volecore/diag.h"

VoleType parse_type(Parser* p) {
  Token tok = ts_next(&p->ts);
  if (tok.kind == KW_INT)
    return (VoleType){.kind = TK_PRIMITIVE, .pt = PT_INT};

  throw_diag(p->engine, tok.span, CC_PRS_EXPECTED_TYPE);
}
