#ifndef __STRING_INTERNER_H
#define __STRING_INTERNER_H

#include "core/infvec.h"
#include "core/stringdef.h"
#include "core/vmem_arena.h"
#include "core/vec.h"
#include <stdint.h>

#define DEFAULT_INTERNER_RESIZE_RATIO 0.8
#define DEFAULT_STRING_INTERNER_CAP 1024
typedef uint32_t InternID;

typedef struct InternEntry {
  uint32_t intern_id;
} InternEntry;

typedef vec(InternEntry) InternEntryVec;

typedef struct StringInterner {
  InternEntryVec entries;
  infvec(uint32_t) offsets; 
  InternID cap; // power of 2
  InternID len;
  VMEMArena *arena;
} StringInterner;

StringInterner* interner_new(VMEMArena *arena);
InternID intern(StringView span, StringInterner *interner);
bstr interner_fetch_str(StringInterner* interner, InternID id);
void interner_free(StringInterner *interner);

#endif
