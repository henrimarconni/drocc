#include "chasm/diag.h"

const DiagInfo chasm_diaginfos[__chasm_diaginfos_len] = {
#define X(_, str, level) {level, str},
    ERRORS(X)
#undef X
};
