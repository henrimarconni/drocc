#include "scanner.h"
#include "span.h"
#include "vmem_arena.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int nextch(SourceFile* file) {
  int ch = file->contents[file->pos.id++];
  if (ch == '\n') {
    file->pos.row++;
    file->pos.col = 0;
  } else if (ch == '\0') {
    file->pos.id--;
    return EOF;
  } else
    file->pos.col++;

  return ch;
}

int peekch(SourceFile* file) {
  if (file->pos.id >= file->pos.len)
    return EOF;
  int ch = file->contents[file->pos.id];
  return ch == '\0' ? EOF : ch;
}

int peeknextch(SourceFile* file) {
  if (file->pos.id + 1 >= file->pos.len)
    return EOF;
  int ch = file->contents[file->pos.id + 1];
  return ch == '\0' ? EOF : ch;
}

void skip_space(SourceFile* file) {
  while (isspace(peekch(file)))
    nextch(file);
}

SourceFile read_file(VMEMArena* arena, bstr confpath) {
  FILE* file = fopen(confpath, "r");
  if (!file) {
    fprintf(stderr, ANSI_RED "Error: " ANSI_RESET "Cannot read the file %s\n", confpath);
    exit(-1);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  SourceFile sf = {};
  sf.pos.len = file_size;
  sf.name = confpath;
  sf.contents = vmarena_alloc(arena, file_size + 1);
  size_t n = fread(sf.contents, 1, file_size, file);
  assert(n == (size_t)file_size);
  sf.contents[file_size] = '\0';

  fclose(file);
  return sf;
}
