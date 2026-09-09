#include "chucci_sem/symbol.h"
#include "core/string_interner.h"

Symbol sym_fetch(SymbolTable* table, InternID id) { return table->symbols.get[id]; }
