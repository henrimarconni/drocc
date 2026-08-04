#include "core/clexer_utils.h"
#include "core/scanner.h"
#include "core/srcman.h"
#include <stdio.h>

int skip_c_comments(SrcScanner* scanner) {
  while (peekch(scanner) == '/') {
    if (peeknextch(scanner) == '/') {
      char ch;
      while ((ch = peekch(scanner)) != '\n') {
        ch = nextch(scanner);
        if (ch == EOF)
          return -1;
      }
    } else if (peeknextch(scanner) == '*') {
      nextch(scanner);           // skip /
      char ch = nextch(scanner); // skip *
      while (true) {
        ch = nextch(scanner);
        if (ch == '*' && peekch(scanner) == '/') {
          nextch(scanner); // skip /
          break;
        }
        if (ch == EOF)
          return -1;
      }
    } else
      break;
  }
  return 0;
}

int lex_cstr(SrcScanner* scanner) {
  if (peekch(scanner) != '"')
    return -1;

  // consume opening "
  nextch(scanner);
  while (true) {
    switch (nextch(scanner)) {
    case '"':
      return 0;

    case '\\':
      if (nextch(scanner) == EOF)
        return -1;
      break;

    case '\n':
    case EOF:
      return -1;

    default:
      break;
    }
  }
}
