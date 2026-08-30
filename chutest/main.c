#include "chucci_lex/lexer.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/parser.h"
#include "chucci_preproc/preproc.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <setjmp.h>

int main(int argc, char** argv) {
  assert(argc == 2);

  VMEMArena* arena = vmarena_new(1024 * 1024);
  StringInterner* interner = interner_new(arena);
  SourceManager* sman = sman_new();
  SrcScanner scanner = sman_str(sman, "chutest", argv[1], strlen(argv[1]));
  PPSearchPaths paths = {NULL, 0};

  jmp_buf onerror;

  if (setjmp(onerror) == 0) {
    TokenStream lexerstream = lexer_new(sman, scanner, interner, arena, &onerror);
    TokenStream preprocstream =
        preproc_new(lexerstream, sman, interner, paths, paths, arena, &onerror);
    Parser parser = parser_new(preprocstream, sman, interner, arena);

    parse_next(&parser);
  }
}
