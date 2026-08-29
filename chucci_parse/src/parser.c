#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/decl_spec.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include <assert.h>

Parser parser_new(TokenStream ts, SourceManager* sman, StringInterner* interner, VMEMArena* arena) {
  Parser p = {0};
  p.arena = arena;
  p.sman = sman;
  p.ts = ts;
  p.interner = interner;
  p.tyint = ty_interner_new();
  p.scratch = vmarena_new(128 * 1024);

  return p;
}

// static Block parse_block(Parser* p) {}

ASTNode* parse_next(Parser* p) {
  TypeID tyid;
  StorageClass sc;
  parse_decl_specifier(p, &tyid, &sc);

  Declarator* decl = parse_declarator(p);
  print_decl(p, decl);

  tyid = unwind_declarator(decl, p, tyid);

  Token token = ts_peek(&p->ts);

  // Function
  if (decl->kind == DECL_FUNCTION) {
  }

  ASTNode a = {0};
  return &a;
}
