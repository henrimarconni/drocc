#ifndef __LEXER_H
#define __LEXER_H

#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "core/diagnostics.h"
#include "core/scanner.h"

typedef struct {
  SourceFile file;
  DiagEngine* engine;
} Lexer;

TokenStream lexer_new(SourceFile file);

/// Do not call this directly, use ts_next
Token lexer_next(void* lexer);
/// Do not call this directly, use ts_peek
Token lexer_peek(void* lexer);


#endif
