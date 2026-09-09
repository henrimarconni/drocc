#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "chucci_parse/typeinterner.h"
#include "chucci_sem/scope.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include <stddef.h>

typedef uint32_t SymbolID;

typedef struct {
  TypeID tyid;
  ScopeID scope;
  InternID intid;
} Symbol;

typedef struct {
  vec(Symbol) symbols;
  uint32_t len, cap;
} SymbolTable;

SymbolTable* symtable_new();
SymbolID add_sym(SymbolTable* table, InternID intid, TypeID tyid, ScopeID scope);
Symbol sym_fetch(SymbolTable* table, SymbolID id);

#endif
