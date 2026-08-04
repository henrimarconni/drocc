#ifndef MM_DIAG_H
#define MM_DIAG_H

#include "core/diagnostics.h"


#define ERRORS(X) \
X(MM_ERR_UNEXPECTED_TOK, "Expected `%s` found `%s`", DL_ERROR)\
X(MM_ERR_UNEXPECTED_IFACE_TYPE, "Expected one of `Dynamic` or `Static`, found `%sv`", DL_ERROR)\
X(MM_ERR_UNEXPECTED_EOF, "Unexpected EOF", DL_ERROR)\
X(MM_ERR_UNEXPECTED_CHAR, "Unexpected character %c", DL_ERROR)\
X(MM_ERR_UNEXPECTED_KEYW, "Unexpected keyword %sv, expected one of `$interface`, `$export`, `$impl`", DL_ERROR)\
X(MM_ERR_CANT_OPEN_FILE, "Cannot open file: %s", DL_ERROR)\
X(MM_ERR_HEADER_FILE_NOT_IN_DOUBLE_QUOTES, "Header file must be in `\"` `\"` or `<` `>` and shouldn't have spaces: found %sv", DL_ERROR)\
\
X(MM_ERR_FN_NOT_DEFINED_BUT_REFERENCED, "Function %sv is not defined in interface %sv but is referenced in implementation %sv", DL_ERROR)\
X(MM_ERR_IMPL_NOT_DEFINED, "No implementation %sv found for interface %sv", DL_ERROR)\
\
X(MM_ERR_IMPL_ALREADY_EXISTS, "Implementation named %sv already exists in interface %sv", DL_ERROR)\
X(MM_ERR_INTERFACE_ALREADY_EXISTS, "Interface %sv already exists", DL_ERROR)\
X(MM_ERR_INTERFACE_DOESNT_EXIST, "Interface %sv doesn't exist", DL_ERROR)\
\
X(MM_NOTE_OVERRIDING_EXPORT, "Overriding `export %sv as %sv` with `export %sv as %sv`", DL_NOTE)


typedef enum {
#define X(a, _, __) a,
  ERRORS(X)
#undef X
__mm_diagtype_len
} MMDiagType;

extern const DiagInfo mm_diaginfos[__mm_diagtype_len];

#endif
