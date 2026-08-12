#include "chucci_parse/decl_spec.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include <assert.h>

static TypeKind resolve_decl_spec_primitive(DeclSpecBuilder* dsb) {
  assert(dsb->num_signed == 0 || dsb->num_unsigned == 0);

  assert(
      dsb->num_void <= 1 && dsb->num_bool <= 1 && dsb->num_char <= 1 && dsb->num_short <= 1 &&
      dsb->num_int <= 1 && dsb->num_float <= 1 && dsb->num_double <= 1 && dsb->num_signed <= 1 &&
      dsb->num_unsigned <= 1);

  assert(dsb->num_long <= 2);
  assert(dsb->num_long == 0 || dsb->num_short == 0);

  // Resolve type
  if (dsb->num_void) {
    // void combined with something else
    assert(dsb->total_primitives <= 1);
    return TY_VOID;
  }
  if (dsb->num_bool) {
    // bool combined with something else
    assert(dsb->total_primitives <= 1);
    return TY_BOOL;
  }
  if (dsb->num_char) {
    assert(
        !dsb->num_short && !dsb->num_long && !dsb->num_int && !dsb->num_float && !dsb->num_double);

    return dsb->num_unsigned ? TY_U8 : TY_I8;
  }
  if (dsb->num_float) {
    assert(dsb->total_primitives <= 1);
    return TY_F32;
  }
  if (dsb->num_double) {
    assert(
        !dsb->num_short && !dsb->num_int && !dsb->num_char && !dsb->num_signed &&
        !dsb->num_unsigned);

    if (dsb->num_long == 1)
      return TY_FL;

    // long long double
    assert(dsb->num_long <= 1);
    return TY_F64;
  }

  // long long
  if (dsb->num_long == 2)
    return dsb->num_unsigned ? TY_U64 : TY_I64;
  // long
  else if (dsb->num_long == 1)
    // (Windows, fk you)
    return dsb->num_unsigned ? TY_U64 : TY_I64;
  // short
  else if (dsb->num_short == 1)
    return dsb->num_unsigned ? TY_U16 : TY_I16;
  // int
  else
    return dsb->num_unsigned ? TY_U32 : TY_I32;
}

static void resolve_decl_spec(Parser* p, DeclSpecBuilder* dsb, TypeID* tyid, StorageClass* sc) {
  *sc = dsb->storage_class;
  Type type = {0};
  type.qual.is_const = dsb->is_const;
  type.qual.is_restrict = dsb->is_restrict;
  type.qual.is_volatile = dsb->is_volatile;

  if (dsb->total_primitives >= 1) {
    type.qual.kind = resolve_decl_spec_primitive(dsb);
    *tyid = ty_intern(p->ty_int, type.qual, NULL, 0);
    return;
  }

  assert(false && "struct...etc arent implemented yet");
}

static bool try_qualifier(TokenKind kind, DeclSpecBuilder* tb) {
  switch (kind) {
  case KW_CONST:
    tb->is_const = true;
    return true;
  case KW_VOLATILE:
    tb->is_volatile = true;
    return true;
  case KW_RESTRICT:
    tb->is_restrict = true;
    return true;
  default:
    return false;
  }
}

static bool try_storage_class(TokenKind kind, DeclSpecBuilder* tb) {
  switch (kind) {
  case KW_STATIC:
    tb->storage_class = SC_STATIC;
    return true;
  case KW_EXTERN:
    tb->storage_class = SC_EXTERN;
    return true;
  case KW_TYPEDEF:
    tb->storage_class = SC_TYPEDEF;
    return true;
  default:
    return false;
  }
}

static bool try_primitive(TokenKind kind, DeclSpecBuilder* tb) {
  switch (kind) {
  case KW_VOID:
    tb->num_void++;
    break;
  case KW_BOOL:
    tb->num_bool++;
    break;
  case KW_CHAR:
    tb->num_char++;
    break;
  case KW_SHORT:
    tb->num_short++;
    break;
  case KW_INT:
    tb->num_int++;
    break;
  case KW_LONG:
    tb->num_long++;
    break;
  case KW_FLOAT:
    tb->num_float++;
    break;
  case KW_DOUBLE:
    tb->num_double++;
    break;
  case KW_SIGNED:
    tb->num_signed++;
    break;
  case KW_UNSIGNED:
    tb->num_unsigned++;
    break;
  default:
    return false;
  }
  tb->total_primitives++;
  return true;
}

void parse_decl_specifier(Parser* p, TypeID* tyid, StorageClass* sc) {
  DeclSpecBuilder dsb = {0};

  while (true) {
    Token token = ts_peek(&p->ts);
    if (try_qualifier(token.kind, &dsb))
      ts_next(&p->ts);
    else if (try_storage_class(token.kind, &dsb))
      ts_next(&p->ts);
    else if (try_primitive(token.kind, &dsb))
      ts_next(&p->ts);
    // try_struct_enum_union(p, &dsb) and try_typedef(p, &dsb)
    else
      break;
  }

  resolve_decl_spec(p, &dsb, tyid, sc);
  *sc = dsb.storage_class;
}
