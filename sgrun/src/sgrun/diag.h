#ifndef SG_DIAG_H_
#define SG_DIAG_H_

#include "core/diag_core.h"

#define ERRORS(X)\
X(SGR_CLI_CANNOT_OPEN_TEST_LIB, "Cannot open test library: %s", DL_ERROR)\
X(SGR_CLI_INVALID_ARG, "Invalid argument", DL_ERROR)\
X(SGR_CLI_LIB_NOT_SPECIFIED, "Test library not specified", DL_ERROR)\
X(SGR_CLI_EXTRA_LIB_SPECIFIED, "Extra test libraries specified", DL_ERROR)\
X(SGR_CLI_RUN_FN_INVALID_CMD, "Invalid arguments to --run-fn", DL_ERROR)\
X(SGR_CLI_PROCESS_ERROR, "", DL_ERROR)\

typedef enum {
#define X(a, _, __) a,
ERRORS(X)
#undef X
__sgr_diaginfos_len
} SGRCliErrors;

extern const DiagInfo sgr_diaginfos[__sgr_diaginfos_len];

#endif
