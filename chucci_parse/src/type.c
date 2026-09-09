#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include <assert.h>
#include <stdio.h>

void print_func_type(Parser* p, Type* type) {
  // Parameters start at index 1 and come in pairs (name, type)

  putc('(', stdout);

  for (uint8_t i = 1; i < type->payload_len; i += 2) {
    if (i > 1) {
      printf(", ");
    }

    InternID param_name = *(InternID*)&type->payload[i];
    TypeID param_ty = *(TypeID*)&type->payload[i + 1];
    Type* param_type = ty_fetch(p->tyint, param_ty);

    if (param_name != 0)
      printf("%s: ", interner_fetch_str(p->interner, param_name));
    else
      printf("<unnamed>: ");

    print_type(p, param_type);
  }

  // Return type is at index 0
  TypeID ret_ty = *(TypeID*)&type->payload[0];
  Type* ret_type = ty_fetch(p->tyint, ret_ty);
  printf(") -> ");
  print_type(p, ret_type);
}

void print_type(Parser* p, Type* type) {
  switch (type->kind) {
#define X(a, b)                                                                                    \
  case a:                                                                                          \
    fputs(b, stdout);                                                                              \
    break;
    TY_PRIMITIVES(X)
#undef X

  case TY_FUNCTION:
    printf("fn(");
    print_func_type(p, type);
    putc(')', stdout);
    break;
  case TY_INCOMPLETE_ARRAY:
    printf("array");
    break;
  case TY_POINTER:
    Type* target = ty_fetch(p->tyint, *(TypeID*)&type->payload[0]);
    print_type(p, target);
    printf("*");
    break;
  default:
    assert(false && "NOT IMPLEMENTED");
  }
}
