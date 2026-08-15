#include "chucci_lex/cc_diag.h"

const DiagInfo cclex_diaginfos[] = {
#define X(_, msg, level) {level, msg},
    CHUCCI_LEX_DIAGS(X)
#undef X
};
