#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static Declarator* make_decl(Parser* p, DeclaratorKind kind) {
  Declarator* decl = vmarena_alloc(p->arena, sizeof(Declarator));
  *decl = (Declarator){0};
  decl->kind = kind;
  return decl;
}

static Declarator* parse_declarator_direct(Parser* p) {
  Declarator* decl = NULL;
  Token token = ts_peek(&p->ts);

  if (token.kind == TOK_IDENT) {
    decl = make_decl(p, DECL_IDENT);
    decl->ident = token.ident;
    ts_next(&p->ts); // skip
  }

  // grouped types like (*fn)
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
      token = ts_next(&p->ts);

      // incomplete array
      if (token.kind == SEP_RSQ) {
        Declarator* arr = make_decl(p, DECL_INCOMPLETE_ARRAY);
        arr->inner = decl;
        decl = arr;
      }
      // complete array (with number of elements specified)
      else
        assert(false && "Complete arrays arent implemented yet");
    }
    // Function
    else if (token.kind == SEP_LPAREN) {
      Declarator* func = make_decl(p, DECL_FUNCTION);
      func->inner = decl;
      decl = func;
      assert(ts_next(&p->ts).kind == SEP_RPAREN);
    }
    token = ts_peek(&p->ts);
  }

  return decl;
}

Declarator* parse_declarator(Parser* p) {
  Token token = ts_peek(&p->ts);

  if (token.kind == OP_MUL) {
    ts_next(&p->ts);
    Declarator* decl = make_decl(p, DECL_POINTER);

    while (true) {
      TokenKind tk = ts_peek(&p->ts).kind;
      if (tk == KW_CONST)
        decl->ptrqual.is_const = 1;
      else if (tk == KW_VOLATILE)
        decl->ptrqual.is_volatile = 1;
      else if (tk == KW_RESTRICT)
        decl->ptrqual.is_restrict = 1;
      else
        break;
      ts_next(&p->ts);
    }

    decl->inner = parse_declarator(p);
    return decl;
  }

  return parse_declarator_direct(p);
}

TypeID unwind_declarator(Declarator* decl, Parser* p, TypeID current) {
  while (decl) {
    switch (decl->kind) {
    case DECL_IDENT: {
      assert(decl->inner == NULL);
      break;
    }
    case DECL_FUNCTION: {
      // param1_ty, param2_ty ..... return_ty
      vec_push(decl->params, current);
      current = ty_intern(
          p->ty_int, tyqual(TY_FUNCTION, false, false, false), decl->params.get, decl->params.n);
      break;
    }
    case DECL_POINTER: {
      uint32_t payload[] = {current};
      decl->ptrqual.kind = TY_POINTER;
      current = ty_intern(p->ty_int, decl->ptrqual, payload, 1);
      break;
    }
    case DECL_INCOMPLETE_ARRAY: {
      uint32_t payload[] = {current};
      current = ty_intern(p->ty_int, tyqual(TY_INCOMPLETE_ARRAY, false, false, false), payload, 1);
      break;
    }
    default:
      print_decl(p, decl);
      assert(false && "TODO");
    }
    decl = decl->inner;
  }

  return current;
}

void print_decl(Parser* p, Declarator* decl) {
  while (decl) {
    switch (decl->kind) {
    case DECL_INCOMPLETE_ARRAY:
      printf("INC_ARRAY");
      break;
    case DECL_ARRAY:
      printf("ARRAY");
      break;
    case DECL_IDENT:
      printf("IDENT(%s)", interner_fetch_sv(p->interner, decl->ident).str);
      break;
    case DECL_FUNCTION:
      printf("FUNCTION");
      break;
    case DECL_POINTER:
      printf("POINTER");
      break;
    }
    decl = decl->inner;
    if (decl)
      printf(" -> ");
  }
  printf("\n");
  fflush(stdout);
}
