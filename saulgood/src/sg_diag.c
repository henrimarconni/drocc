#include "core/diagnostics.h"
#include "saulgood/sg_diag.h"

const DiagInfo sg_diaginfos[__sg_diagtype_len] = {
#define X(_, str, level) {level, str},
    ERRORS(X)
#undef X
};
