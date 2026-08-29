#ifndef SCOPE_H_
#define SCOPE_H_

#include "chucci_parse/symbol.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include <stdint.h>

typedef uint16_t ScopeID;

typedef struct {
  InternID ident;
  Symbol symbol;
} InternIDSymbolPair;

typedef struct {
  vec(Symbol) symbols; // internid -> symbol map (for faster global lookups)
} GlobalScope;

typedef struct {
  vec(InternIDSymbolPair) pairs; // a linear map
} LocalScope;

typedef struct Scope Scope;

struct Scope {
  union {
    LocalScope local;
    GlobalScope global;
  };

  // if parent == NULL, then it is global
  Scope* parent;
};

Symbol find_sym(InternID id);
bool is_scope_global(Scope* scope);

#endif
