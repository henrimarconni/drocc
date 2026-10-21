#ifndef __LEXER_H
#define __LEXER_H

#include "lexer/token.h"
#include "lexer/token_stream.h"
#include "core/diagnostics.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"

typedef struct Lexer {
  SrcScanner scanner;
  SourceManager* sman;
  StringInterner* interner;
  DiagEngine* engine;
  // used to check if we are in a preprocessor directive
  // if we are, we emit TOK_NEWLINE for preprocessor smartly
  bool in_pp_directive;
} Lexer;

TokenStream lexer_new(SourceManager* sman, SrcScanner scanner, StringInterner* interner, VMEMArena* arena, DiagEngine* engine);
Token lexer_next(void* lexer);
Token lexer_peek(void* lexer);
void lexer_free(void** lexer);
void lexer_reset(void* lexer);

#endif
