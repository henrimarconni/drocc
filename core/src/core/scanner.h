#ifndef SCANNER_H
#define SCANNER_H

#include "core/stringdef.h"
#include "core/vmem_arena.h"
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

typedef enum {
  SE_OK,
  SE_ERR_CANT_OPEN_FILE,
  SE_ERR_IO,
} ScannerRes;

int nextch(SourceFile* file);
int peekch(SourceFile* file);
int peeknextch(SourceFile* file);
bool match_str(SourceFile* file, bstr str);
void skip_space(SourceFile* file);
ScannerRes read_file(SourceFile* sf, VMEMArena* arena, bstr confpath);

#endif
