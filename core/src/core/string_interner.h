#ifndef __STRING_INTERNER_H
#define __STRING_INTERNER_H

#include "core/stringdef.h"
#include "core/vmem_arena.h"
#include "core/vec.h"
#include <stdint.h>

#define DEFAULT_INTERNER_RESIZE_RATIO 0.8
#define DEFAULT_STRING_INTERNER_CAP 1024
typedef uint32_t InternID;

typedef struct InternEntry {
  /**
    As we assume max size needed by arena will be less than 1 MiB, that is
    1024*1024 = 2^20.
    Offset to InternedStr in arena
  */
  uint64_t offset : 23;
  /// first char of string for faster comparisons
  uint64_t char1 : 8;
  uint64_t is_full : 1;
  uint64_t intern_id : 32;
} InternEntry;

typedef vec(InternEntry) InternEntryVec;

typedef struct StringInterner {
  InternEntryVec entries;
  size_t cap; // power of 2
  size_t len;
  VMEMArena *arena;
} StringInterner;

StringInterner* interner_new(VMEMArena *arena);
InternID intern(StringView span, StringInterner *interner);
void interner_free(StringInterner *interner);

#endif
