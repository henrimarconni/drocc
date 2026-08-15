#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

SourceManager* sman_new(void) {
  SourceManager* sman = malloc(sizeof(SourceManager));
  *sman = (SourceManager){0};
  return sman;
}

SMSpanInfo sman_info(SourceManager* sman, Span span) {
  assert(span.srcid < sman->sources.n);
  SMSource* src = &sman->sources.get[span.srcid];

  SMSpanInfo info = {0};
  info.id = span.offset;
  info.sv = span_sv(sman, span);
  info.file = src;

  if (src->offsets.n == 0) {
    info.row = 1;
    info.col = span.offset;
    return info;
  }

  uint32_t start = 0;
  uint32_t end = src->offsets.n - 1;
  uint32_t row = 0;

  while (start <= end) {
    uint32_t mid = start + (end - start) / 2;
    uint32_t line_start_offset = src->offsets.get[mid];

    if (line_start_offset <= info.id) {
      row = mid;
      start = mid + 1;
    } else
      end = mid - 1;
  }

  info.row = row + 1;
  info.col = info.id - src->offsets.get[row] + 1;

  return info;
}

SrcScanner sman_str(SourceManager* man, const bstr name, const bstr contents, size_t len) {
  // check if file is already loaded
  for (uint16_t i = 0; i < man->sources.n; i++) {
    if (strcmp(man->sources.get[i].name, name) == 0)
      return scanner_new(man, i);
  }

  SMSource source = {
      .len = len, .name = name, .b_contents = contents, .is_mmaped = false, .offsets = {0}};

  // line 1
  vec_push(source.offsets, 0);

  SrcID srcid = (SrcID)man->sources.n;
  vec_push(man->sources, source);

  return scanner_new(man, srcid);
}

bool sman_open(SrcScanner* out_scanner, SourceManager* man, bstr name) {
  // check if file is already loaded
  for (uint16_t i = 0; i < man->sources.n; i++) {
    if (strcmp(man->sources.get[i].name, name) == 0) {
      *out_scanner = scanner_new(man, i);
      return true;
    }
  }

  // load the file
  size_t file_size;
  ostr contents = os_mmap_file(name, &file_size);

  // add it to the list
  SMSource source = {
      .len = file_size, .name = name, .b_contents = contents, .is_mmaped = true, .offsets = {0}};

  // line 1
  vec_push(source.offsets, 0);

  vec_push(man->sources, source);
  *out_scanner = sman_str(man, name, contents, file_size);
  man->sources.get[out_scanner->id].is_mmaped = true;

  return true;
}

void sman_free(SourceManager* sman) {
  for (size_t i = 0; i < sman->sources.n; i++) {
    SMSource* src = &sman->sources.get[i];
    vec_destroy(src->offsets);
    if (src->is_mmaped)
      os_unmap_file(src->o_contents, src->len);
  }
  vec_destroy(sman->sources);
  free(sman);
}
