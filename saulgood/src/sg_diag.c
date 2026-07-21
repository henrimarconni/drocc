#include "diagnostics.h"
#include "sg_diag.h"

const DiagInfo sg_diaginfos[__sg_diagtype_len] = {
#define X(_, str, level) (DiagInfo){level, str},
    ERRORS(X)
#undef X
};
