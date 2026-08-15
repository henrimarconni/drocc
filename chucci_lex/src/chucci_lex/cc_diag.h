#ifndef CC_DIAG_H
#include "core/diagnostics.h"

#define CHUCCI_LEX_DIAGS(X)\
X(CC_ERR_UNEXPECTED_TOKEN, "Unexpected token, expected %s found %s", DL_ERROR)

typedef enum {
#define X(a, _, __) a,
CHUCCI_LEX_DIAGS(X)
#undef X
} CCLexDiagType;

extern const DiagInfo cclex_diaginfos[];

#endif
