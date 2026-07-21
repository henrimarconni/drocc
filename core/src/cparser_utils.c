#include "assert.h"
#include "cparser_utils.h"
#include "scanner.h"
#include "span.h"

void skip_c_comments(SourceFile* file, Span* span) {
  while (peekch(file) == '/') {
    if (peeknextch(file) == '/') {
      char ch;
      while ((ch = peekch(file)) != '\n') {
        ch = nextch(file);
        assert(ch != EOF);
        span->len++;
      }
    } else if (peeknextch(file) == '*') {
      nextch(file); // skip /
      span->len++;
      nextch(file); // skip *
      span->len++;
      char ch = peekch(file);
      while (peekch(file) != '*' || peeknextch(file) != '/') {
        nextch(file);
        assert(ch != EOF);
        span->len++;
      }
      nextch(file); // skip *
      span->len++;
      nextch(file); // skip /
      span->len++;
    } else
      break;
  }
}

Span parse_cstr(SourceFile* file) {
  Span span = span_from_file(file);
  nextch(file);
  span.str++; // skip "
  int ch = peekch(file);
  while (ch != '"') {
    assert(ch != EOF);
    nextch(file);
    ch = peekch(file);
    span.len++;
  }
  nextch(file); // skip ending "
  return span;
}
