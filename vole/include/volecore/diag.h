#ifndef VOLE_DIAG_H_
#define VOLE_DIAG_H_

#include "core/diag_core.h"

#define VOLE_ERRORS(X)                                                                             \
  X(CC_LEX_INVALID_C_BLOCK_COMMENT, "Invalid block comment", DL_ERROR)                             \
  X(CC_LEX_INVALID_STRING, "Invalid string", DL_ERROR)                                             \
  X(CC_LEX_UNEXPECTED_CHAR, "Unexpected character", DL_ERROR)                                      \
  X(CC_ERR_UNEXPECTED_TOKEN, "Unexpected token, expected %s found %s", DL_ERROR)                   \
  X(CC_PRS_EXPECTED_TYPE, "Expected type", DL_ERROR)                                               \
  X(CC_PRS_EXPECTED_STMT, "Expected statement", DL_ERROR)                                          \
  X(CC_PRS_EXPECTED_EXPR, "Expected expression", DL_ERROR)

typedef enum {
#define X(a, _, __) a,
  VOLE_ERRORS(X)
#undef X
      __vole_diaginfos_len
} VoleErrorType;

extern const DiagInfo vole_diaginfos[__vole_diaginfos_len];

#endif
