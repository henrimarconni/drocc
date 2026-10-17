#include "core/diagnostics.h"
#include "volecore/diag.h"

const DiagInfo vole_diaginfos[__vole_diaginfos_len] = {
#define X(_, str, level) {level, str},
    VOLE_ERRORS(X)
#undef X
};
