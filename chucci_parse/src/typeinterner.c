#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/slice.h"
#include "core/vec.h"
#include "thirdparty/wyhash.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY_TYPEID 0
#define TY_RESIZE_RATIO 0.80

#define hash_bytes(bytes, len) wyhash((bytes), (len), 0, _wyp)
#define is_pow_2(n) ((n & (n - 1)) == 0)
#define entry_str(entry, interner) (char*)((interner)->arena->data + (entry)->offset)
#define wrap_around(num, cap) (num & (cap - 1))

#define DEFAULT_TY_INTERNER_CAP 1024

static_assert(is_pow_2(DEFAULT_TY_INTERNER_CAP), "Interner capacity must be power of 2");

TypeInterner* ty_interner_new() {
  TypeInterner* interner = malloc(sizeof(TypeInterner));
  *interner = (TypeInterner){0};
  interner->cap = DEFAULT_TY_INTERNER_CAP;

  // reserve 0 as EMPTY_TYPEid
  interner->len = 1;
  vec_push(interner->types, (Type){0});

  vec_resize(interner->entries, DEFAULT_TY_INTERNER_CAP);
  memset(interner->entries.get, 0, DEFAULT_TY_INTERNER_CAP * sizeof(TypeIntrnEntry));
  return interner;
}

static uint64_t hash_type(Type* type) {
  // prepare the data for hashing
  uint32_t buf[64] = {0};
  uint32_t* data = buf;

  if (type->payload.len + 1 > sizeof(buf) / sizeof(uint32_t))
    data = malloc(32 * (type->payload.len + 1));

  data[0] = type->kind;
  memcpy(data + 1, type->payload.data, type->payload.len * 4);

  if (type->payload.len + 1 > sizeof(buf) / sizeof(uint32_t))
    free(data);

  return hash_bytes(data, type->payload.len + 1);
}

static bool type_cmp(Type* type1, Type* type2) {
  return type1->kind == type2->kind && type1->is_const == type2->is_const &&
         type1->is_restrict == type2->is_restrict && type1->is_volatile == type2->is_volatile &&
         type1->payload.len == type2->payload.len &&
         memcmp(type1->payload.data, type2->payload.data, type1->payload.len * sizeof(uint32_t));
}

static TypeIntrnEntry* find_entry(TypeInterner* interner, Type* type) {
  uint64_t hash = hash_type(type);
  uint64_t id = wrap_around(hash, interner->cap);
  TypeIntrnEntry* entry = &interner->entries.get[id];

  if (entry->typeid == EMPTY_TYPEID)
    return entry;

  // already interned
  Type* entry_type = &interner->types.get[entry->typeid];
  if (type_cmp(type, entry_type))
    return entry;

  // linear probing
  while (entry->typeid != EMPTY_TYPEID) {
    Type* entry_type = &interner->types.get[entry->typeid];
    if (type_cmp(type, entry_type))
      return entry;

    id = wrap_around(id + 1, interner->cap);
    entry = &interner->entries.get[id];
  }
  return entry;
}

static void resize(TypeInterner* interner) {
  TypeIntrnEntryVec old_entries = interner->entries;
  interner->entries = (TypeIntrnEntryVec){0};
  interner->cap *= 2;

  vec_resize(interner->entries, interner->cap);
  memset(interner->entries.get, 0, interner->entries.n * sizeof(TypeIntrnEntry));

  for (size_t i = 0; i < old_entries.n; i++) {
    TypeIntrnEntry* old_entry = &old_entries.get[i];
    if (old_entry == EMPTY_TYPEID)
      continue;
    Type* type = &interner->types.get[old_entry->typeid];
    TypeIntrnEntry* new_entry = find_entry(interner, type);
    *new_entry = *old_entry;
  }

  vec_destroy(old_entries);
}

TypeID ty_intern(TypeInterner* interner, Type* type) {
  if (interner->len >= TY_RESIZE_RATIO * interner->cap)
    resize(interner);

  TypeIntrnEntry* entry = find_entry(interner, type);

  entry->typeid = interner->len++;
  entry->kind = type->kind;
  vec_push(interner->types, *type);
  return entry->typeid;
}

Type ty_fetch(TypeInterner* interner, TypeID id) { return interner->types.get[id]; }
