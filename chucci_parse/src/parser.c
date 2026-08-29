#include "chucci_parse/decl_spec.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "core/vmem_arena.h"

Parser* parser_new(TokenStream ts, SourceManager* sman, VMEMArena* arena) {
  Parser* p = vmarena_calloc(arena, sizeof(Parser));
  p->arena = arena;
  p->sman = sman;
  p->ts = ts;

  return p;
}

ASTNode* parse_next(Parser* p) {
  TypeID tyid;
  StorageClass sc;
  parse_decl_specifier(p, &tyid, &sc);

  Declarator* decl = parse_declarator(p);

  ASTNode a = {0};
  return &a;
}
