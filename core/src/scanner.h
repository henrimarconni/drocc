#ifndef SCANNER_H
#define SCANNER_H

#include "stringdef.h"
#include "vmem_arena.h"
#include <stddef.h>
#define EOF (-1)

typedef struct {
  size_t row, col, id, len;
} Position;

typedef struct {
  bstr name;
  bstr contents;
  Position pos;
} SourceFile;

int nextch(SourceFile* file);
int peekch(SourceFile* file);
int peeknextch(SourceFile* file);
void skip_space(SourceFile* file);
SourceFile read_file(VMEMArena* arena, bstr confpath);

#endif
