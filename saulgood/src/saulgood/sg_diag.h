#ifndef SG_DIAG_H
#define SG_DIAG_H


#include "core/diagnostics.h"
#define ERRORS(X)\
X(ERR_CANT_OPEN_FILE, "Cannot open file: %s", DL_ERROR)\
X(ERR_UNEXPECTED_EOF, "Unexpected End-Of-File", DL_ERROR)\
X(ERR_UNEXPECTED_TOK, "Expected %s, found %span", DL_ERROR)\
X(ERR_UNEXPECTED_CHAR, "Unexpected character: %span found", DL_ERROR)\
X(ERR_UNEXPECTED_KEYW, "Unexpected keyword %span, expected one of `$c` or `$test`", DL_ERROR)


typedef enum {
#define X(a, _, __) a,
ERRORS(X)
#undef X
__sg_diagtype_len
} SGDiagType;

extern const DiagInfo sg_diaginfos[__sg_diagtype_len];


#endif
