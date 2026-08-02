#include "core/span.h"
#include "core/srcman.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

SourceManager sman_new() {
  SourceManager sman = {0};
  return sman;
}

SMSpanInfo sman_info(SourceManager* sman, Span span) {
  assert(span.srcid < sman->sources.n);
  SMSource* src = &sman->sources.get[span.srcid];

  SMSpanInfo info = {0};
  info.id = span.offset;
  info.sv = span_sv(sman, span);
  info.file = src;

  // Binary search to find row and column
  size_t start = 0;
  size_t end = sman->sources.n - 1;
  size_t mid;
  while (true) {
    mid = (start + (end - start) / 2);
    printf("Searching: %zu - %zu - %zu\n", start, mid, end);

    uint32_t off1 = src->offsets.get[mid];
    if (mid == sman->sources.n - 1)
      break;
    uint32_t off2 = src->offsets.get[mid + 1];
    if (info.id >= off1 && info.id <= off2)
      break;

    if (info.id > off1)
      start = mid;
    else
      end = mid;
  }
  printf("Found: %zu\n", mid);

  info.row = mid;
  info.col = src->offsets.get[mid] - info.id;

  return info;
}

SrcID sman_open(SourceManager* man, bstr name, VMEMArena* arena) {
  // check if file is already loaded
  for (size_t i = 0; i < man->sources.n; i++) {
    if (strcmp(man->sources.get[i].name, name) == 0)
      return i;
  }

  FILE* file = fopen(name, "r");
  if (!file)
    return INVALID_SRC_ID;

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return INVALID_SRC_ID;
  }

  long file_size = ftell(file);
  if (file_size < 0) {
    fclose(file);
    return INVALID_SRC_ID;
  }
  rewind(file);

  VMEMArenaMark mark = vmarena_mark(arena);

  size_t pathlen = strlen(name);
  bstr dup = vmarena_alloc(arena, pathlen + 1);
  char* contents = vmarena_alloc(arena, file_size + 1);

  if (!dup || !contents) {
    vmarena_mark_reset(arena, mark);
    fclose(file);
    return INVALID_SRC_ID;
  }

  // Read data
  size_t n = fread(contents, 1, file_size, file);
  fclose(file);

  if (n != (size_t)file_size) {
    vmarena_mark_reset(arena, mark);
    return INVALID_SRC_ID;
  }

  // Finalize string copies and initialize struct
  memcpy(dup, name, pathlen);
  dup[pathlen] = '\0';
  contents[file_size] = '\0';

  SMSource source = {.len = file_size, .name = dup, .contents = contents};

  // Push to vector
  SrcID srcid = man->sources.n;
  vec_push(man->sources, source);

  return srcid;
}

void sman_free(SourceManager* sman) { vec_destroy(sman->sources); }
