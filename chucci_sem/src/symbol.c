#include "chucci_sem/symbol.h"
#include "core/vec.h"
#include "thirdparty/wyhash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define hash_bytes(bytes, len) wyhash((bytes), (len), 0, _wyp)
#define is_pow_2(n) (((n) & ((n) - 1)) == 0)
#define wrap_around(num, cap) ((num) & ((cap) - 1))
#define INITIAL_SYMBOLS_CAP 1024

static_assert(is_pow_2(INITIAL_SYMBOLS_CAP), "SymbolTable capacity must be power of 2");

Symbol sym_fetch(SymbolTable* table, SymbolID id) { return table->symbols.get[id]; }

static bool is_sym_empty(Symbol* sym) {
  Symbol empty = {0};
  return memcmp(sym, &empty, sizeof(Symbol)) == 0;
}

static bool sym_cmp(Symbol* sym1, Symbol* sym2) { return memcmp(sym1, sym2, sizeof(Symbol)) == 0; }

SymbolTable* symtable_new() {
  SymbolTable* table = malloc(sizeof(SymbolTable));
  memset(table, 0, sizeof(SymbolTable));
  vec_resize(table->symbols, INITIAL_SYMBOLS_CAP);

  table->cap = INITIAL_SYMBOLS_CAP;
  table->len = 0;

  memset(table->symbols.get, 0, sizeof(Symbol) * table->symbols.m);
  return table;
}

SymbolID add_sym(SymbolTable* table, InternID intid, TypeID tyid, ScopeID scope) {
  Symbol data = {0};
  data.intid = intid;
  data.scope = scope;
  data.tyid = tyid;

  uint64_t hash = hash_bytes(&data, sizeof(Symbol));

  uint32_t id = wrap_around(hash, table->cap);
  Symbol* sym = &table->symbols.get[id];

  while (!is_sym_empty(sym)) {
    if (sym_cmp(sym, &data))
      break;

    id = wrap_around(id + 1, table->cap);
    sym = &table->symbols.get[id];
  }

  *sym = data;
  return id;
}
