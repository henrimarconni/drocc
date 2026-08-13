#ifndef TYPEINTERNER_H_
#define TYPEINTERNER_H_

#include "chucci_parse/type.h"
#include "core/infvec.h"
#include "core/vec.h"
#include <assert.h>
#include <stdint.h>

typedef uint32_t TypeID;

typedef struct Type Type;

typedef vec(TypeID) TypeIDVec;

typedef struct TypeInterner {
  TypeID len;
  TypeID cap;
  TypeIDVec entries;
  infvec(Type) types;
  infvec(uint32_t) payloads;
} TypeInterner;

TypeInterner* ty_interner_new();
TypeID ty_intern(TypeInterner* interner, TyQualifier qual, uint32_t* payload, uint32_t count);

Type ty_fetch(TypeInterner* interner, TypeID id);
void* ty_payload_base(TypeInterner* interner, uint32_t offset);

void tyint_free(TypeInterner* interner);

#endif
