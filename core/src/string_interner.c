#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "thirdparty/wyhash.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define hash_str(bytes, len) wyhash((bytes), (len), 0, _wyp)
#define is_pow_2(n) ((n & (n - 1)) == 0)
#define entry_str(entry, interner) (char*)((interner)->arena->data + (entry)->offset)
#define wrap_around(num, cap) (num & (cap - 1))

StringInterner* interner_new(VMEMArena* arena) {
  // calloc gives zeroed memory
  StringInterner* interner = vmarena_calloc(arena, sizeof(StringInterner));
  interner->cap = DEFAULT_STRING_INTERNER_CAP;
  assert(is_pow_2(interner->cap));

  vec_resize(interner->entries, interner->cap);

  // we use memset instead of looping through each of these
  memset(interner->entries.get, 0, interner->entries.n * sizeof(InternEntry));

  interner->arena = arena;
  return interner;
}

static inline InternID
populate(InternEntry* entry, StringInterner* interner, bstr str, size_t len) {
  entry->is_full = true;
  entry->char1 = str[0];
  entry->intern_id = interner->len++;

  bstr new_str = vmarena_alloc(interner->arena, len + 1);
  entry->offset = (size_t)(new_str - (char*)interner->arena->data);

  memcpy(new_str, str, len);
  new_str[len] = '\0';
  return entry->intern_id;
}

static inline InternEntry* find_entry(StringInterner* interner, bstr str, size_t len) {
  uint64_t hash = hash_str(str, len);
  uint64_t id = wrap_around(hash, interner->cap);
  InternEntry* entry = &interner->entries.get[id];

  if (!entry->is_full)
    return entry;

  // Already interned
  if (str[0] == entry->char1 && memcmp(str + 1, entry_str(entry, interner) + 1, len - 1) == 0)
    return entry;

  // linear probing
  while (entry->is_full) {
    if (str[0] == entry->char1 && memcmp(str + 1, entry_str(entry, interner) + 1, len - 1) == 0)
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
  memset(interner->entries.get, 0, interner->entries.n * sizeof(InternEntry));

  for (size_t i = 0; i < old_entries.n; i++) {
    InternEntry* old_entry = &old_entries.get[i];
    if (!old_entry->is_full)
      continue;
    bstr str = entry_str(old_entry, interner);
    size_t len = strlen(str);

    InternEntry* new_entry = find_entry(interner, str, len);
    *new_entry = *old_entry;
  }

  vec_destroy(old_entries);
}

InternID intern(StringView strv, StringInterner* interner) {
  assert(strv.len > 0);

  if (interner->len >= DEFAULT_INTERNER_RESIZE_RATIO * interner->cap)
    resize(interner);

  InternEntry* entry = find_entry(interner, strv.str, strv.len);
  if (entry->is_full)
    return entry->intern_id;
  return populate(entry, interner, strv.str, strv.len);
}

void interner_free(StringInterner* interner) { vec_destroy(interner->entries); }
