#ifndef AMAL_DIAG_H
#define AMAL_DIAG_H

#include "diagnostics.h"
#define AMAL_ERRORS(X)\
X(AMAL_ERR_INVALID_STR, "Invalid string: %span", DL_ERROR)

typedef enum {
#define X(a, _, __) a,
AMAL_ERRORS(X)
#undef X
__amal_diaginfos_len
} AmalErrorType;

extern const DiagInfo amal_diaginfos[__amal_diaginfos_len];

#endif
