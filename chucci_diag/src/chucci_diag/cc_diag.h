#ifndef CC_DIAG_H
#include "core/diagnostics.h"

#define CHUCCI_DIAGS(X)                                                                            \
  X(CC_ERR_UNEXPECTED_TOKEN, "Unexpected token, expected %s found %s", DL_ERROR)                   \
  X(CC_LEX_UNEXPECTED_CHAR, "Unexpected character: %c", DL_ERROR)                                  \
  X(CC_LEX_INVALID_C_BLOCK_COMMENT, "Unterminated Block Comment", DL_ERROR)\
  X(CC_LEX_INVALID_STRING, "Unterminated C string", DL_ERROR)

typedef enum {
#define X(a, _, __) a,
  CHUCCI_DIAGS(X)
#undef X
      _cc_diaginfos_len
} CCDiagType;

extern const DiagInfo cc_diaginfos[_cc_diaginfos_len];

#endif
