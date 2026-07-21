#include "scanner.h"
#include <ctype.h>

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
