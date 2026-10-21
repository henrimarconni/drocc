#ifndef SCOPE_H_
#define SCOPE_H_

#include "core/string_interner.h"
#include "parser/type.h"
#include <stdint.h>

typedef struct {
  vec(VoleType) vars;
} Scope;

Scope new_scope();
bool get_var_type(VoleType* type, Scope* scope, InternID id);
void register_var(Scope* scope, VoleType type, InternID id);

#endif
