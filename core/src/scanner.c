#include "core/scanner.h"
#include "core/srcman.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

SrcScanner scanner_new(SourceManager* sman, SrcID id) {
  SrcScanner scanner = {0};
  scanner.sman = sman;
  scanner.srcid = id;
  return scanner;
}

int nextch(SrcScanner* scanner) {
  SMSource file = scanner->sman->sources.get[scanner->srcid];
  int ch = file.b_contents[scanner->id++];
  if (ch == '\0') {
    scanner->id--;
    return EOF;
  }
  if (ch == '\n')
    vec_push(scanner->sman->sources.get[scanner->srcid].offsets, scanner->id);

  return ch;
}

bool match_str(SrcScanner* scanner, bstr str) {
  SrcScanner bak = *scanner;
  while (*str) {
    if (nextch(scanner) != *str) {
      *scanner = bak;
      return false;
    }
    str++;
  }
  return true;
}

int peekch(SrcScanner* scanner) {
  assert(scanner->sman);
  assert(scanner->srcid < scanner->sman->sources.n);
  SMSource file = scanner->sman->sources.get[scanner->srcid];
  if (scanner->id >= file.len)
    return EOF;
  int ch = file.b_contents[scanner->id];
  return ch == '\0' ? EOF : ch;
}

int peeknextch(SrcScanner* scanner) {
  SMSource file = scanner->sman->sources.get[scanner->srcid];
  if (scanner->id + 1 >= file.len)
    return EOF;
  int ch = file.b_contents[scanner->id + 1];
  return ch == '\0' ? EOF : ch;
}

void skip_space(SrcScanner* scanner) {
  int ch = peekch(scanner);
  while (ch != EOF && isspace(ch)) {
    nextch(scanner);
    ch = peekch(scanner);
  }
}

Span span_begin(SrcScanner* scanner) {
  Span span = {.offset = scanner->id, .len = 0, .srcid = scanner->srcid};
  return span;
}

void span_end(Span* span, SrcScanner* scanner) { span->len = scanner->id - span->offset; }
SrcScanner scanner_new(SourceManager* sman, SrcID id);

void scanner_rewind(SrcScanner* scanner, Span span) { scanner->id = span.offset; }
