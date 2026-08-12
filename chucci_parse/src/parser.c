#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdio.h>

Parser parser_new(TokenStream ts, VMEMArena* arena, SourceManager* sman, StringInterner* interner) {
  Parser p = {0};
  p.arena = arena;
  p.sman = sman;
  p.ts = ts;
  p.interner = interner;
  vec_resize(p.symbols, interner->len);

  return p;
}

Declarator* make_decl(Parser* p, DeclaratorKind kind) {
  Declarator* decl = vmarena_alloc(p->arena, sizeof(Declarator));
  decl->kind = kind;
  decl->inner = NULL;
  return decl;
}

Declarator* parse_declarator(Parser* p);

Declarator* parse_declarator_direct(Parser* p) {
  Declarator* decl = NULL;
  Token token = ts_peek(&p->ts);

  if (token.kind == TOK_IDENT) {
    decl = make_decl(p, DECL_IDENT);
    decl->ident = token.ident;
    ts_next(&p->ts); // skip
  }
  // grouped thing like (*fn)
  else if (token.kind == SEP_LPAREN) {
    ts_next(&p->ts);
    decl = parse_declarator(p);
    assert(ts_peek(&p->ts).kind == SEP_RPAREN);
    ts_next(&p->ts);
  }

  token = ts_peek(&p->ts);
  while (token.kind == SEP_LSQ || token.kind == SEP_LPAREN) {
    ts_next(&p->ts); // skip [ or (

    // Array
    if (token.kind == SEP_LSQ) {
      Declarator* arr = make_decl(p, DECL_ARRAY);
      arr->inner = decl;
      decl = arr;
    }
    // Function
    else if (token.kind == SEP_LPAREN) {
      Declarator* func = make_decl(p, DECL_FUNCTION);
      func->inner = decl;
      decl = func;
    }
    ts_next(&p->ts); // skip ] or )
    token = ts_peek(&p->ts);
  }

  return decl;
}

Declarator* parse_declarator(Parser* p) {
  Token token = ts_peek(&p->ts);

  if (token.kind == OP_MUL) {
    ts_next(&p->ts); // *

    Declarator* decl = make_decl(p, DECL_POINTER);
    decl->inner = parse_declarator(p);
    return decl;
  }

  return parse_declarator_direct(p);
}

void parse_decl_specifier(Parser* p, Type* base, StorageClass* sc) {
  *base = (Type){0};
  base->kind = TY_INCOMPLETE;

  bool should_loop = true;
  while (should_loop) {
    Token token = ts_peek(&p->ts);

    switch (token.kind) {
    case KW_INT: {
      ts_next(&p->ts); // skip token
      if (base->kind == TY_INCOMPLETE)
        base->kind = TY_I32;
      else
        assert(false);
      break;
    }
    case KW_CONST: {
      ts_next(&p->ts); // skip token
      base->is_const = true;
      break;
    }
    case KW_STATIC: {
      ts_next(&p->ts); // skip token
      *sc = SC_STATIC;
      break;
    }
    default: {
      should_loop = false;
      break;
    }
    }
  }
}

TopLevelNode parser_next(Parser* p) {
  Type base;
  StorageClass sc;

  parse_decl_specifier(p, &base, &sc);
  Declarator* decl = parse_declarator(p);

  while (decl) {
    switch (decl->kind) {
    case DECL_ARRAY:
      printf("ARRAY ");
      break;
    case DECL_IDENT:
      printf("IDENT(%s) ", interner_fetch_sv(p->interner, decl->ident).str);
      break;
    case DECL_FUNCTION:
      printf("FUNCTION ");
      break;
    case DECL_POINTER:
      printf("POINTER ");
      break;
    }
    decl = decl->inner;
  }

  printf("\n");

  fflush(stdout);
  assert(false);
}
