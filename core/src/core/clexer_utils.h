/**
  @file
  Utilities for lexing c code.
  @note To be replaced by chucci_lex
*/

#ifndef CLEXER_UTILS_H
#define CLEXER_UTILS_H
#include "core/scanner.h"

typedef enum {
  CLEX_OK,
  CLEX_INVALID_STR,
  CLEX_UNEXPECTED_EOF
} CLexerRes;

/// Tries to skip c comments, returns error if there is some unknown syntax
CLexerRes skip_c_comments(SourceFile* file);
CLexerRes lex_cstr(SourceFile* file);

#endif
