#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "chucci_parse/typeinterner.h"
#include "core/infvec.h"
#include "core/string_interner.h"

typedef struct {
  TypeID type;
} Symbol;

typedef struct {
  infvec(Symbol) symbols;
} SymbolTable;

Symbol sym_fetch(SymbolTable* table, InternID id);

#endif
