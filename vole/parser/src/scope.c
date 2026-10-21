#include "core/string_interner.h"
#include "core/vec.h"
#include "parser/scope.h"
#include "parser/type.h"
#include <stdint.h>

#define DEFAULT_VAR_COUNT 128

Scope new_scope() { return (Scope){}; }

bool get_var_type(VoleType* type, Scope* scope, InternID id) {
  if (scope->vars.m <= id)
    return false;

  // TODO: Var exists check
  *type = scope->vars.get[id];
  return true;
}

void register_var(Scope* scope, VoleType type, InternID id) {
  while (scope->vars.m < id) {
    uint32_t cap = scope->vars.m == 0 ? DEFAULT_VAR_COUNT : scope->vars.m * 2;
    vec_resize(scope->vars, cap);
  }

  scope->vars.get[id] = type;
}
