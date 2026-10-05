#ifndef DEFINE_C_
#define DEFINE_C_

#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include <stdint.h>

typedef struct {
  Token* start;
  uint32_t len;
  uint32_t og_len;
} MacroStream;

typedef struct {
  InternID id;
  TokenStream ts;
} MacroPair;

typedef struct  {
  vec(MacroPair);
} MacroManager;

typedef struct Preprocessor Preprocessor;
void define_macro(Preprocessor* pp);

MacroManager* macroman_new();
void macroman_free(MacroManager* man);
void macroman_add(MacroManager* man, InternID name, TokenStream ts);
void macroman_remove(MacroManager *man, InternID name);

Token ms_next(void* ms);
Token ms_peek(void* ms);
void ms_free(void** ms);
void ms_reset(void* ms);

#endif
