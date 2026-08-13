#include "core/infvec.h"
#include "core/string_interner.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "thirdparty/wyhash.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define EMPTY_INTERNID 0
#define DEFAULT_SIZE 1024 * 1024

#define hash_str(bytes, len) wyhash((bytes), (len), 0, _wyp)
#define is_pow_2(n) ((n & (n - 1)) == 0)
#define entry_str(entry, interner) (char*)((interner)->arena->data + (entry)->offset)
#define wrap_around(num, cap) (num & (cap - 1))

StringInterner* interner_new(VMEMArena* arena) {
  // calloc gives zeroed memory
  StringInterner* interner = vmarena_calloc(arena, sizeof(StringInterner));
  interner->cap = DEFAULT_STRING_INTERNER_CAP;
  assert(is_pow_2(interner->cap));

  // 0 reserved as empty internid
  interner->len++;
  infvec_init(interner->offsets, DEFAULT_SIZE);
  infvec_push(interner->offsets, 0);

  vec_resize(interner->entries, interner->cap);

  // we use memset instead of looping through each of these
  memset(interner->entries.get, 0, interner->cap * sizeof(InternEntry));

  interner->arena = arena;
  return interner;
}

static inline InternID
populate(InternEntry* entry, StringInterner* interner, bstr str, size_t len) {
  bstr new_str = vmarena_alloc(interner->arena, len + 1);
  memcpy(new_str, str, len);
  new_str[len] = '\0';

  uint32_t offset = (new_str - (char*)interner->arena->data);

  entry->intern_id = interner->len++;

  infvec_push(interner->offsets, offset);
  return entry->intern_id;
}

static inline InternEntry* find_entry(StringInterner* interner, bstr str, size_t len) {
  uint64_t hash = hash_str(str, len);
  uint64_t id = wrap_around(hash, interner->cap);
  InternEntry* entry = &interner->entries.get[id];

  if (entry->intern_id == EMPTY_INTERNID)
    return entry;

  StringView sv = {str, len};

  // linear probing
  while (entry->intern_id != EMPTY_INTERNID) {
    // Already interned
    bstr stored = interner_fetch_str(interner, entry->intern_id);
    if (memcmp(str, stored, len) == 0 && stored[len] == '\0')
      return entry;

    id = wrap_around(id + 1, interner->cap);
    entry = &interner->entries.get[id];
  }
  return entry;
}

void resize(StringInterner* interner) {
  InternEntryVec old_entries = interner->entries;
  interner->entries = (InternEntryVec){0};
  interner->cap *= 2;

  vec_resize(interner->entries, interner->cap);
  memset(interner->entries.get, 0, interner->cap * sizeof(InternEntry));

  for (size_t i = 0; i < old_entries.n; i++) {
    InternEntry* old_entry = &old_entries.get[i];
    if (old_entry->intern_id == EMPTY_INTERNID)
      continue;

    bstr str = interner_fetch_str(interner, old_entry->intern_id);
    InternEntry* new_entry = find_entry(interner, str, strlen(str));
    *new_entry = *old_entry;
  }

  vec_destroy(old_entries);
}

InternID intern(StringView strv, StringInterner* interner) {
  assert(strv.len > 0);

  if (interner->len >= DEFAULT_INTERNER_RESIZE_RATIO * interner->cap)
    resize(interner);

  InternEntry* entry = find_entry(interner, strv.str, strv.len);
  if (entry->intern_id != EMPTY_INTERNID)
    return entry->intern_id;

  return populate(entry, interner, strv.str, strv.len);
}

bstr interner_fetch_str(StringInterner* interner, InternID id) {
  assert(id != 0);
  return (char*)interner->arena->data + interner->offsets.get[id];
}

void interner_free(StringInterner* interner) {
  infvec_destroy(interner->offsets);
  vec_destroy(interner->entries);
}
