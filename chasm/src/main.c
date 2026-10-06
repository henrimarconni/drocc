#include "chu80/lexer.h"
#include "chu80/parser.h"
#include "core/srcman.h"
#include <assert.h>
#include <setjmp.h>
#include <string.h>

int main(int argc, char** argv) {
  assert(argc == 2);
  SourceManager* sman = sman_new();
  SrcScanner scanner = sman_str(sman, "chasm", argv[1], strlen(argv[1]));
  jmp_buf onerror;

  if (setjmp(onerror) == 0) {
    C80Lexer l = c80lex_new(scanner, sman, &onerror);
    C80Parser p = c80_new(l);
    c80_pass1(&p);
    c80_pass2(&p);
  }
}
