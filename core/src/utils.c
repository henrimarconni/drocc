#include "stringdef.h"
#include "utils.h"
#include "vmem_arena.h"
#include <assert.h>
#include <stdlib.h>

ostr read_file(VMEMArena* arena, bstr confpath) {
  FILE* file = fopen(confpath, "r");
  if (!file) {
    fprintf(stderr, ANSI_RED "Error: " ANSI_RESET "Cannot read the file %s\n", confpath);
    exit(-1);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  ostr source = vmarena_alloc(arena, file_size + 1);
  size_t n = fread(source, 1, file_size, file);
  assert(n == (size_t)file_size);
  source[file_size] = '\0';

  fclose(file);

  return source;
}
