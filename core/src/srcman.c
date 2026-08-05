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

  size_t start = 0;
  size_t end = src->offsets.n - 1;
  size_t row = 0;

  while (start <= end) {
    size_t mid = start + (end - start) / 2;
    uint32_t line_start_offset = src->offsets.get[mid];

    if (line_start_offset <= info.id) {
      row = mid;
      start = mid + 1;
    } else
      end = mid - 1;
  }

  info.row = row + 1;
  info.col = info.id - src->offsets.get[row];

  return info;
}

SrcID sman_str(SourceManager* man, bstr name, bstr contents, size_t len) {
  SMSource source = {0};
  source.len = len;
  source.name = name;
  source.contents = contents;

  // Calculate offsets
  size_t offset = 0;
  // first line
  vec_push(source.offsets, 0);
  for (bstr curr = contents; curr < contents + len; curr++) {
    if (*curr == '\n')
      vec_push(source.offsets, offset);
    offset++;
  }

  // Push to vector
  SrcID srcid = man->sources.n;
  vec_push(man->sources, source);

  return srcid;
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

  return sman_str(man, name, contents, file_size);
}

void sman_free(SourceManager* sman) { vec_destroy(sman->sources); }
