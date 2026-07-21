#include "mm_diag.h"

const DiagInfo mm_diaginfos[__mm_diagtype_len] = {
#define X(_, str, level) (DiagInfo){level, str},
    ERRORS(X) NOTES(X)
#undef X
};
