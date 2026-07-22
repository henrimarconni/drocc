#ifndef CPARSER_UTILS_H
#define CPARSER_UTILS_H
#include "scanner.h"

typedef enum {
  CLEX_OK,
  CLEX_UNEXPECTED_EOF
} CLexerRes;

CLexerRes skip_c_comments(SourceFile* file);
CLexerRes lex_cstr(SourceFile* file);

#endif
