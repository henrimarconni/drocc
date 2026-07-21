#ifndef MM_DIAG_H
#define MM_DIAG_H

#include "diagnostics.h"


#define ERRORS(X) \
X(ERR_UNEXPECTED_TOK, "Expected `%s` found `%span`", DL_ERROR)\
X(ERR_UNEXPECTED_KEYW, "Unexpected keyword %span, expected one of `$interface`, `$export`, `$impl`", DL_ERROR)\
X(ERR_CANT_OPEN_FILE, "Cannot open file: %s", DL_ERROR)\
X(ERR_INTERFACE_DOESNT_EXIST, "Interface %span doesn't exist", DL_ERROR)\
X(ERR_HEADER_FILE_NOT_IN_DOUBLE_QUOTES, "Header file must be in double quotes and shouldn't have spaces: found %span", DL_ERROR)\
X(ERR_FN_NOT_DEFINED_BUT_REFERENCED, "Function %span is not defined in interface %span but is referenced in implementation %span", DL_ERROR)\
X(ERR_IMPL_NOT_DEFINED, "No implementation %span found for interface %span", DL_ERROR)\
X(ERR_IMPL_ALREADY_EXISTS, "Implementation named %span already exists in interface %span", DL_ERROR)\
X(ERR_INTERFACE_ALREADY_EXISTS, "Interface %span already exists", DL_ERROR)\
X(ERR_INVALID_IDENTIFIER, "Expected a identifier, found %span", DL_ERROR)\
X(ERR_INVALID_STRING, "Expected a string, found %span", DL_ERROR)

#define NOTES(X)\
X(NOTE_OVERRIDING_EXPORT_CLI, "Overriding `export %span as %span` with `export %span as %span` from command-line argument `--export`", DL_NOTE)\
X(NOTE_OVERRIDING_EXPORT_CONF, "Overriding `export %span as %span` with `export %span as %span` from configuration", DL_NOTE)


typedef enum {
#define X(a, _, __) a,
  ERRORS(X)
  NOTES(X)
#undef X
__mm_diagtype_len
} MMDiagType;

extern DiagInfo mm_diaginfos[__mm_diagtype_len];

#endif
