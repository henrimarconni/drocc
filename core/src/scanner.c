#include "core/scanner.h"
#include "core/vmem_arena.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  Position pos = file->pos;
  while (*str) {
    if (nextch(file) != *str) {
      file->pos = pos;
      return false;
    }
    str++;
  }
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

  size_t pathlen = strlen(confpath);
  bstr dup = vmarena_alloc(arena, pathlen + 1);
  memcpy(dup, confpath, pathlen);
  dup[pathlen] = '\0';

  sf->pos.len = file_size;
  sf->name = dup;
  sf->contents = vmarena_alloc(arena, file_size + 1);
  size_t n = fread(sf->contents, 1, file_size, file);
  if (n != (size_t)file_size) {
    fclose(file);
    return SE_ERR_IO;
  }
  sf->contents[file_size] = '\0';

  fclose(file);
  return SE_OK;
}
