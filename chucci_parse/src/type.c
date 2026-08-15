#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include <stdio.h>

void print_type(TypeID tyid, TypeInterner* tyint) {
  if (tyid == 0) {
    printf("<empty_type>");
    return;
  }
  Type type = ty_fetch(tyint, tyid);

  uint32_t* payload = ty_payload_base(tyint, type.payload.offset);

  switch (type.qual.kind) {
  // --- PRIMITIVES ---
  case TY_VOID:
    printf("void");
    break;
  case TY_BOOL:
    printf("bool");
    break;

  case TY_I8:
    printf("char");
    break;
  case TY_U8:
    printf("unsigned char");
    break;
  case TY_I16:
    printf("short");
    break;
  case TY_U16:
    printf("unsigned short");
    break;
  case TY_I32:
    printf("int");
    break;
  case TY_U32:
    printf("unsigned int");
    break;
  case TY_I64:
    printf("long");
    break;
  case TY_U64:
    printf("unsigned long");
    break;

  case TY_F32:
    printf("float");
    break;
  case TY_F64:
    printf("double");
    break;
  case TY_FL:
    printf("long double");
    break;

  case TY_POINTER:
    // [TargetTypeID]
    printf("(");
    print_type(payload[0], tyint);
    printf(")*");
    break;

  case TY_ARRAY:
    // [TargetTypeID, ArrayLength]
    print_type(payload[0], tyint);
    printf("[%u]", payload[1]);
    break;

  case TY_INCOMPLETE_ARRAY:
    // [TargetTypeID]
    printf("(");
    print_type(payload[0], tyint);
    printf(")[]");
    break;

  case TY_FUNCTION:
    // [Param1TypeID, Param2TypeID, ..., ReturnTypeID]
    printf("fn (");
    if (type.payload.len == 1)
      printf("void");
    else {
      for (uint32_t i = 0; i < type.payload.len - 1; i++) {
        print_type(payload[i], tyint);
        if (i < type.payload.len - 1)
          printf(", ");
      }
    }
    printf(") -> ");
    print_type(payload[type.payload.len - 1], tyint); // Print return type
    break;

  default:
    printf("<unknown>");
    break;
  }
}

TyQualifier tyqual(TypeKind kind, bool is_const, bool is_volatile, bool is_restrict) {
  return (TyQualifier){
      .kind = kind, .is_const = is_const, .is_restrict = is_restrict, .is_volatile = is_volatile};
}
