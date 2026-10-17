#ifndef STRMAP_H_
#define STRMAP_H_

#include "core/infvec.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define EMPTY_MAPID 0
#define DEFAULT_STRMAP_CAP 1024
#define DEFAULT_STRMAP_RESIZE_RATIO 0.75

typedef uint32_t MapID;

typedef struct {
  MapID map_id;
} MapEntry;

typedef vec(MapEntry) MapEntryVec;

typedef struct {
  size_t cap;
  size_t len;
  infvec(uint32_t) offsets; // Arena offsets for the keys
  infvec(int) values;       // Parallel array for integer values
  MapEntryVec entries;     // Hash table of IDs
  VMEMArena* arena;
} StrToIntMap;

StrToIntMap* strmap_new(VMEMArena* arena);
void strmap_put(StrToIntMap* map, StringView strv, int value);
bool strmap_get(StrToIntMap* map, StringView strv, int* out_value);
void strmap_free(StrToIntMap* map);

#endif
