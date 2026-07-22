#include "clexer_utils.h"
#include "scanner.h"
#include <stdio.h>

CLexerRes skip_c_comments(SourceFile* file) {
  while (peekch(file) == '/') {
    if (peeknextch(file) == '/') {
      char ch;
      while ((ch = peekch(file)) != '\n') {
        ch = nextch(file);
        if (ch == EOF)
          return CLEX_UNEXPECTED_EOF;
      }
    } else if (peeknextch(file) == '*') {
      nextch(file);           // skip /
      char ch = nextch(file); // skip *
      while (true) {
        ch = nextch(file);
        if (ch == '*' && peekch(file) == '/') {
          nextch(file); // skip /
          break;
        }
        if (ch == EOF)
          return CLEX_UNEXPECTED_EOF;
      }
    } else
      break;
  }
  return CLEX_OK;
}

CLexerRes lex_cstr(SourceFile* file) {
  nextch(file); // consume opening '"'
  for (;;) {
    switch (nextch(file)) {
    case '"':
      return CLEX_OK;

    case '\\':
      if (nextch(file) == EOF)
        return CLEX_UNEXPECTED_EOF;
      break;

    case '\n':
    case EOF:
      return CLEX_UNEXPECTED_EOF;

    default:
      break;
    }
  }
}
