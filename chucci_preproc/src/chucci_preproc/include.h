#ifndef INCLUDE_H_
#define INCLUDE_H_

#include "chucci_lex/token_stream.h"
#include "preproc.h"

/**
  Expects TOK_STR  or TOK_ANGSTR
  @return TokenStream of the included file
*/
TokenStream preproc_parse_include(Preprocessor* pp);

#endif
