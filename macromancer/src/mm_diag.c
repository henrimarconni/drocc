#include "macromancer/mm_diag.h"

const DiagInfo mm_diaginfos[__mm_diagtype_len] = {
#define X(_, str, level) {level, str},
    ERRORS(X)
#undef X
};
