#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "chucci_parse/type.h"
#include "core/srcman.h"
#include "core/string_interner.h"

typedef uint32_t SymbolID;

typedef enum {
  SYM_FUNCTION,
  SYM_VARIABLE,
  SYM_TYPEDEF,

  // for later: SYM_MODULE
} SymbolKind;

typedef struct SymbolData {
  Span span;
  InternID name;
  Type type;
  SymbolKind kind;
} SymbolData;

#endif
