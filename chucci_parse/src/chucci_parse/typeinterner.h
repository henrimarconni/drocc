#ifndef TYPEINTERNER_H_
#define TYPEINTERNER_H_

#include "chucci_parse/type.h"
#include "core/vec.h"
#include <assert.h>
#include <stdint.h>

typedef uint32_t TypeID;

static_assert(
    _type_kind_count < 32,
    "There cannot be more than 31 unique typekinds, or else the TypeInterner will break"
    "(modify it yourself if you want)");

typedef struct {
  uint32_t typeid : 27;
  uint32_t kind : 5;
} TypeIntrnEntry;

typedef vec(TypeIntrnEntry) TypeIntrnEntryVec;

typedef struct {
  TypeID len;
  TypeID cap;
  TypeIntrnEntryVec entries;
  vec(Type) types;
} TypeInterner;

TypeInterner* ty_interner_new();
TypeID ty_intern(TypeInterner* interner, Type* type);
Type ty_fetch(TypeInterner* interner, TypeID id);

#endif
