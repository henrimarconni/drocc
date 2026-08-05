#ifndef __LEXER_H
#define __LEXER_H

#include "chucci_lex/token.h"
#include "core/srcman.h"
#include "core/string_interner.h"

typedef struct Lexer {
  SrcScanner scanner;
  SourceManager* sman;
  StringInterner* interner;
} Lexer;

Lexer lexer_new(SourceManager* sman, SrcID srcid, StringInterner* interner);
Token lexer_next(Lexer* lexer);
Token lexer_peek(Lexer* lexer);

#endif
