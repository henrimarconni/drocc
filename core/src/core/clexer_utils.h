/**
  @file
  Utilities for lexing c code.
  @note To be replaced by chucci_lex
*/

#ifndef CLEXER_UTILS_H
#define CLEXER_UTILS_H
#include "core/srcman.h"

/// @return -1 on failure
int skip_c_comments(SrcScanner* scanner);

/**
  lex string with double quotes
  @return -1 on failure
*/
int lex_cstr(SrcScanner* scanner);

#endif
