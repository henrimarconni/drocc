#include "chucci_lex/token_stream.h"
#include "chucci_parse/decl_spec.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdio.h>

Parser parser_new(
    TokenStream ts,
    VMEMArena* arena,
    SourceManager* sman,
    StringInterner* interner,
    TypeInterner* ty_int) {
  Parser p = {0};
  p.arena = arena;
  p.sman = sman;
  p.ts = ts;
  p.interner = interner;
  p.ty_int = ty_int;
  vec_resize(p.symbols, interner->len);

  return p;
}

TopLevelNode parser_next(Parser* p) {
  TypeID base;
  StorageClass sc;

  VMEMArenaMark mark = vmarena_mark(p->arena);

  parse_decl_specifier(p, &base, &sc);
  Declarator* decl = parse_declarator(p);
  print_decl(p, decl);
  base = unwind_declarator(decl, p, base);

  vmarena_mark_reset(p->arena, mark);

  print_type(base, p->ty_int);
  printf("\n");
  fflush(stdout);

  assert(false);
}
