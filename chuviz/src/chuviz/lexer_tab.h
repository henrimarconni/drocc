#ifndef LEXER_TAB_H_
#define LEXER_TAB_H_

#include "chucci_lex/token.h"
#include "core/srcman.h"
#include "core/vec.h"

typedef struct {
  SourceManager* sman;
  vec(Token) tokens; // flattened TokenStream
} LexerTab;



#endif
