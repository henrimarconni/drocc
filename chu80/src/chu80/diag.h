#ifndef DIAG_H_
#define DIAG_H_

#include "core/diagnostics.h"
#define C80_ERRORS(X)                                                                              \
  X(C80_ERR_FILE_NOT_FOUND, "File %s not found", DL_ERROR)                                         \
  X(C80_ERR_INVALID_CHAR, "Invalid character: %c", DL_ERROR)                                       \
  X(C80_ERR_INVALID_LABEL, "Invaid label: %sv", DL_ERROR)                                          \
  X(C80_ERR_INVALID_MOV, "Invaid mov with 110B", DL_ERROR)                                         \
  X(C80_ERR_LARGER_THAN_1_BYTE,                                                                    \
    "Invalid immediate value: %d, which is larger than 1 byte",                                    \
    DL_ERROR)                                                                                      \
  X(C80_ERR_INVALID_INT, "Invalid integer", DL_ERROR)\
  X(C80_ERR_UNDEFINED_SYMBOL, "Undefined symbol", DL_ERROR)                                        \
  X(C80_ERR_UNEXPECTED_EXPR, "Unexpected expression", DL_ERROR)                                         \
  X(C80_ERR_INVALID_REG, "Invalid register", DL_ERROR)

typedef enum {
#define X(a, _, __) a,
  C80_ERRORS(X)
#undef X
      __c80_diaginfos_len
} C80ErrorType;

extern const DiagInfo c80_diaginfos[__c80_diaginfos_len];

#endif
