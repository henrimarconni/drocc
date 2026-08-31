#ifndef TYPEINTERNER_H_
#define TYPEINTERNER_H_

#include "chucci_parse/type.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>

// THIS MUST BE 32 bits
typedef struct {
  uint32_t id : 29;
  uint32_t is_const : 1;
  uint32_t is_volatile : 1;
  uint32_t is_restrict : 1;
} TypeID;

typedef struct Type Type;

typedef vec(TypeID) TypeIDVec;

typedef struct TypeInterner {
  uint32_t len;
  uint32_t cap;
  TypeIDVec entries;
  // this will store types
  VMEMArena* arena;
} TypeInterner;

TypeInterner* ty_interner_new(void);
TypeID ty_intern(TypeInterner* interner, TypeKind kind, bool is_const, bool is_restrict, bool is_volatile, uint32_t* payload, uint8_t count);
Type* ty_fetch(TypeInterner* interner, TypeID id);
void tyint_free(TypeInterner* interner);

#endif
