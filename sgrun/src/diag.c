#include "sgrun/diag.h"

const DiagInfo sgr_diaginfos[__sgr_diaginfos_len] = {
#define X(_, str, level) {level, str},
    ERRORS(X)
#undef X
};
