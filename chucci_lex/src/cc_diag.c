#include "chucci_lex/cc_diag.h"

const DiagInfo cc_diaginfos[] = {
#define X(_, msg, level) {level, msg},
    CHUCCI_DIAGS(X)
#undef X
};
