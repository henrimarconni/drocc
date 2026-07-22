#include "scanner.h"
#include "vmem_arena.h"
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

bool match_str(SourceFile* file, bstr str) {
  size_t pos = file->pos.id;
  while (*str) {
    if (file->contents[pos] != *str)
      return false;
    pos++;
    str++;
  }
  file->pos.id = pos;
  return true;
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
  int ch = peekch(file);
  while (ch != EOF && isspace(ch)) {
    nextch(file);
    ch = peekch(file);
  }
}

ScannerRes read_file(SourceFile* sf, VMEMArena* arena, bstr confpath) {
  FILE* file = fopen(confpath, "r");
  if (!file)
    return SE_ERR_CANT_OPEN_FILE;
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  if (file_size < 0)
    return SE_ERR_IO;
  rewind(file);

  sf->pos.len = file_size;
  sf->name = confpath;
  sf->contents = vmarena_alloc(arena, file_size + 1);
  size_t n = fread(sf->contents, 1, file_size, file);
  sf->contents[file_size] = '\0';

  fclose(file);
}
