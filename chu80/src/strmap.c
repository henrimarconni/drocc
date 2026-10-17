#include "chu80/strmap.h"
#include "core/vec.h"
#include "thirdparty/wyhash.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_SIZE (1024 * 1024)
#define hash_str(bytes, len) wyhash((bytes), (len), 0, _wyp)
#define is_pow_2(n) (((n) & ((n) - 1)) == 0)
#define wrap_around(num, cap) ((num) & ((cap) - 1))

StrToIntMap* strmap_new(VMEMArena* arena) {
  StrToIntMap* map = vmarena_calloc(arena, sizeof(StrToIntMap));
  map->cap = DEFAULT_STRMAP_CAP;
  assert(is_pow_2(map->cap));

  // 0 reserved as EMPTY_MAPID
  map->len = 1;

  infvec_init(map->offsets, DEFAULT_SIZE);
  infvec_push(map->offsets, 0);

  infvec_init(map->values, DEFAULT_SIZE);
  infvec_push(map->values, 0);

  vec_resize(map->entries, map->cap);
  memset(map->entries.get, 0, map->cap * sizeof(MapEntry));

  map->arena = arena;
  return map;
}

static inline bstr map_fetch_str(StrToIntMap* map, MapID id) {
  assert(id != EMPTY_MAPID);
  return (char*)map->arena->data + map->offsets.get[id];
}

static inline MapEntry* find_entry(StrToIntMap* map, bstr str, size_t len) {
  uint64_t hash = hash_str(str, len);
  uint64_t id = wrap_around(hash, map->cap);
  MapEntry* entry = &map->entries.get[id];

  while (entry->map_id != EMPTY_MAPID) {
    bstr stored = map_fetch_str(map, entry->map_id);

    // Check both length and byte match
    if (strlen(stored) == len && memcmp(str, stored, len) == 0)
      return entry;

    id = wrap_around(id + 1, map->cap);
    entry = &map->entries.get[id];
  }
  return entry;
}

static inline void populate(MapEntry* entry, StrToIntMap* map, bstr str, size_t len, int value) {
  bstr new_str = vmarena_alloc(map->arena, len + 1);
  memcpy(new_str, str, len);
  new_str[len] = '\0';

  uint32_t offset = (uint32_t)(new_str - (char*)map->arena->data);

  entry->map_id = map->len++;

  infvec_push(map->offsets, offset);
  infvec_push(map->values, value);
}

static void resize(StrToIntMap* map) {
  MapEntryVec old_entries = map->entries;
  map->entries = (MapEntryVec){0};

  size_t old_cap = map->cap;
  map->cap *= 2;

  vec_resize(map->entries, map->cap);
  memset(map->entries.get, 0, map->cap * sizeof(MapEntry));

  for (size_t i = 0; i < old_cap; i++) {
    MapEntry* old_entry = &old_entries.get[i];
    if (old_entry->map_id == EMPTY_MAPID)
      continue;

    bstr str = map_fetch_str(map, old_entry->map_id);
    MapEntry* new_entry = find_entry(map, str, strlen(str));
    *new_entry = *old_entry;
  }

  vec_destroy(old_entries);
}

void strmap_put(StrToIntMap* map, StringView strv, int value) {
  if (strv.len == 0)
    return;

  if (map->len >= (size_t)(DEFAULT_STRMAP_RESIZE_RATIO * map->cap))
    resize(map);

  MapEntry* entry = find_entry(map, strv.str, strv.len);

  if (entry->map_id != EMPTY_MAPID) {
    // Key exists, update the mapped value in place
    map->values.get[entry->map_id] = value;
    return;
  }

  // Key is new, populate it and insert value
  populate(entry, map, strv.str, strv.len, value);
}

bool strmap_get(StrToIntMap* map, StringView strv, int* out_value) {
  if (strv.len == 0)
    return false;

  MapEntry* entry = find_entry(map, strv.str, strv.len);
  if (entry->map_id == EMPTY_MAPID)
    return false;

  if (out_value)
    *out_value = map->values.get[entry->map_id];

  return true;
}

void strmap_free(StrToIntMap* map) {
  infvec_destroy(map->offsets);
  infvec_destroy(map->values);
  vec_destroy(map->entries);
}
