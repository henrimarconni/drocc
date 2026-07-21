#include "amalgamate.h"
#include "stringbuilder.h"
#include "utils.h"
#include "vmem_arena.h"

typedef struct {
  StringBuilder output;
  bstr source;
  size_t id;
  VMEMArena* arena;
} Amalgamator;

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, bstr output, bstr input) {
  Amalgamator a = {};
  a.source = read_file(arena, input);
  a.arena = arena;
  a.id = 0;

  return a.output;
}
