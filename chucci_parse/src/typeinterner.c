#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
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
#define wrap_around(num, cap) ((num) & ((cap) - 1))
#define DEFAULT_TY_INTERNER_CAP 1024

static_assert(is_pow_2(DEFAULT_TY_INTERNER_CAP), "Interner capacity must be power of 2");

TypeInterner* ty_interner_new() {
  // initialize base struct
  TypeInterner* interner = malloc(sizeof(TypeInterner));
  *interner = (TypeInterner){0};
  interner->cap = DEFAULT_TY_INTERNER_CAP;

  // reserve 0 as EMPTY_TYPEID and init vectors
  interner->len = 1;
  vec_push(interner->types, (Type){0});
  vec_resize(interner->entries, interner->cap);
  memset(interner->entries.get, 0, interner->cap * sizeof(TypeID));
  return interner;
}

static uint64_t hash_type_query(TyQualifier qual, uint32_t* payload, uint32_t count) {
  uint32_t buf[64] = {0};
  size_t total_count = count + 1;
  uint32_t* data = total_count > 64 ? malloc(total_count * sizeof(uint32_t)) : buf;

  data[0] = qual.kind | (qual.is_const << 5) | (qual.is_volatile << 6) | (qual.is_restrict << 7);
  if (count > 0)
    memcpy(data + 1, payload, count * sizeof(uint32_t));
  uint64_t hash = hash_bytes(data, total_count * sizeof(uint32_t));

  if (total_count > 64)
    free(data);
  return hash;
}

static bool type_cmp(
    TypeInterner* interner, Type* stored, TyQualifier qual, uint32_t* payload, uint32_t count) {
  if (stored->qual.kind != qual.kind || stored->qual.is_const != qual.is_const ||
      stored->qual.is_volatile != qual.is_volatile ||
      stored->qual.is_restrict != qual.is_restrict || stored->payload.len != count)
    return false;

  if (count == 0)
    return true;
  uint32_t* stored_payload = &interner->payloads.get[stored->payload.offset];
  return memcmp(stored_payload, payload, count * sizeof(uint32_t)) == 0;
}

static TypeID*
find_entry(TypeInterner* interner, TyQualifier qual, uint32_t* payload, uint32_t count) {
  uint64_t hash = hash_type_query(qual, payload, count);
  uint64_t id = wrap_around(hash, interner->cap);
  TypeID* entry = &interner->entries.get[id];

  // linear probe
  while (*entry != EMPTY_TYPEID) {
    Type* stored = &interner->types.get[*entry];
    if (type_cmp(interner, stored, qual, payload, count))
      return entry;
    id = wrap_around(id + 1, interner->cap);
    entry = &interner->entries.get[id];
  }
  return entry;
}

static void resize(TypeInterner* interner) {
  TypeIDVec old_entries = interner->entries;
  interner->entries = (TypeIDVec){0};
  interner->cap *= 2;
  vec_resize(interner->entries, interner->cap);
  memset(interner->entries.get, 0, interner->entries.n * sizeof(TypeID));

  // rehash
  for (size_t i = 0; i < old_entries.n; i++) {
    TypeID old_id = old_entries.get[i];
    if (old_id == EMPTY_TYPEID)
      continue;
    Type* stored = &interner->types.get[old_id];
    uint32_t* stored_payload = &interner->payloads.get[stored->payload.offset];
    *find_entry(interner, stored->qual, stored_payload, stored->payload.len) = old_id;
  }
  vec_destroy(old_entries);
}

TypeID ty_intern(TypeInterner* interner, TyQualifier qual, uint32_t* payload, uint32_t count) {
  // resize
  if (interner->len >= TY_RESIZE_RATIO * interner->cap)
    resize(interner);

  TypeID* entry = find_entry(interner, qual, payload, count);
  if (*entry != EMPTY_TYPEID)
    return *entry;

  // populate type
  *entry = interner->len++;
  Type type = {.qual = qual, .payload = {interner->payloads.n, count}};

  for (uint32_t i = 0; i < count; i++)
    vec_push(interner->payloads, payload[i]);
  vec_push(interner->types, type);
  return *entry;
}

void* ty_payload_base(TypeInterner* interner, uint32_t offset) {
  return &interner->payloads.get[offset];
}
Type ty_fetch(TypeInterner* interner, TypeID id) { return interner->types.get[id]; }
