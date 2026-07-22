#include "amal_diag.h"

const DiagInfo amal_diaginfos[__amal_diaginfos_len] = {
#define X(_, msg, level) (DiagInfo){level, msg},
    AMAL_ERRORS(X)
#undef X
};
