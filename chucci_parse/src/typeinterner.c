#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "thirdparty/wyhash.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE (1024 * 1024 * 256)
#define EMPTY_TYPEID 0
#define TY_RESIZE_RATIO 0.80
#define hash_bytes(bytes, len) wyhash((bytes), (len), 0, _wyp)
#define is_pow_2(n) (((n) & ((n) - 1)) == 0)
#define wrap_around(num, cap) ((num) & ((cap) - 1))
#define DEFAULT_TY_INTERNER_CAP 1024

static_assert(is_pow_2(DEFAULT_TY_INTERNER_CAP), "Interner capacity must be power of 2");

TypeInterner* ty_interner_new() {
  TypeInterner* interner = malloc(sizeof(TypeInterner));
  *interner = (TypeInterner){0};
  interner->cap = DEFAULT_TY_INTERNER_CAP;
  interner->arena = vmarena_new(1024 * 1024);

  // Pad the arena by 1 byte so no valid type ever gets offset 0.
  vmarena_alloc(interner->arena, 1);

  interner->len = 0; // Number of unique types
  vec_resize(interner->entries, interner->cap);
  memset(interner->entries.get, 0, interner->cap * sizeof(TypeID));
  return interner;
}

static uint64_t hash_type_query(TypeKind kind, uint32_t* payload, uint32_t count) {
  uint32_t buf[64] = {0};
  uint32_t total_count = count + 1;
  uint32_t* data = total_count > 64 ? malloc(total_count * sizeof(uint32_t)) : buf;

  data[0] = kind;
  if (count > 0)
    memcpy(data + 1, payload, sizeof(uint32_t) * count);

  uint64_t hash = hash_bytes(data, total_count * sizeof(uint32_t));

  if (total_count > 64)
    free(data);
  return hash;
}

static bool type_cmp(Type* stored, TypeKind kind, uint32_t* payload, uint8_t count) {
  if (stored->kind != kind || stored->payload_len != count)
    return false;

  if (count == 0)
    return true;
  return memcmp(stored->payload, payload, count * sizeof(uint32_t)) == 0;
}

Type* ty_fetch(TypeInterner* interner, TypeID tyid) {
  return (Type*)(interner->arena->data + tyid.id);
}

static TypeID* find_entry(TypeInterner* interner, TypeKind kind, uint32_t* payload, uint8_t count) {
  uint64_t hash = hash_type_query(kind, payload, count);
  uint64_t id = wrap_around(hash, interner->cap);
  TypeID* entry = &interner->entries.get[id];

  while (entry->id != EMPTY_TYPEID) {
    Type* stored = ty_fetch(interner, *entry);
    if (type_cmp(stored, kind, payload, count))
      return entry;
    id = wrap_around(id + 1, interner->cap);
    entry = &interner->entries.get[id];
  }
  return entry;
}

static void resize(TypeInterner* interner) {
  TypeIDVec old_entries = interner->entries;

  interner->cap *= 2;
  interner->entries = (TypeIDVec){0};
  vec_resize(interner->entries, interner->cap);
  memset(interner->entries.get, 0, interner->cap * sizeof(TypeID));

  for (size_t i = 0; i < old_entries.n; i++) {
    TypeID old_id = old_entries.get[i];
    if (old_id.id == EMPTY_TYPEID)
      continue;

    Type* stored = ty_fetch(interner, old_id);
    *find_entry(interner, stored->kind, stored->payload, stored->payload_len) = old_id;
  }
  vec_destroy(old_entries);
}

TypeID ty_intern(
    TypeInterner* interner,
    TypeKind kind,
    bool is_const,
    bool is_restrict,
    bool is_volatile,
    uint32_t* payload,
    uint8_t count) {

  if (interner->len >= TY_RESIZE_RATIO * interner->cap)
    resize(interner);

  TypeID* entry = find_entry(interner, kind, payload, count);

  // If the base type doesn't exist, create it
  if (entry->id == EMPTY_TYPEID) {
    uint32_t size = sizeof(Type) + sizeof(uint32_t) * count;
    Type* type = vmarena_alloc(interner->arena, size);
    type->kind = kind;
    type->payload_len = count;
    if (count > 0)
      memcpy(type->payload, payload, count * sizeof(uint32_t));

    entry->id = (uint32_t)((uint8_t*)type - interner->arena->data);
    interner->len++;
  }

  TypeID result = *entry;
  result.is_const = is_const;
  result.is_restrict = is_restrict;
  result.is_volatile = is_volatile;

  return result;
}

void tyint_free(TypeInterner* interner) {
  vec_destroy(interner->entries);
  vmarena_free(interner->arena);
  free(interner);
}
