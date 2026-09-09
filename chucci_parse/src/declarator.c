#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define tyid_to_uint32_t(tyid) *(uint32_t*)(&tyid)

static_assert(sizeof(uint32_t) == sizeof(TypeID));

static Declarator* make_decl(Parser* p, DeclaratorKind kind) {
  Declarator* decl = vmarena_alloc(p->parena, sizeof(Declarator));
  *decl = (Declarator){0};
  decl->kind = kind;
  return decl;
}

static void parse_decl_func_params(Parser* p, Declarator* decl) {
  // Reserve space at index 0 for the return type (filled during unwind)
  vec_push(decl->params, 0);

  Token token = ts_peek(&p->ts);
  while (token.kind != SEP_RPAREN) {
    VarDeclNode node = parse_var_decl(p);

    vec_push(decl->params, node.ident);
    vec_push(decl->params, *(uint32_t*)&node.type);

    token = ts_peek(&p->ts);

    // Handle comma separation between arguments
    if (token.kind == SEP_COMMA) {
      ts_next(&p->ts);
      token = ts_peek(&p->ts);
    } else if (token.kind != SEP_RPAREN) {
      assert(false && "Expected ',' or ')' in function parameters");
    }
  }

  ts_next(&p->ts);
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
      parse_decl_func_params(p, decl);
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

void unwind_declarator(TypeID* tyid, InternID* name, Declarator* decl, Parser* p, TypeID current) {
  while (decl) {
    switch (decl->kind) {
    case DECL_IDENT: {
      *name = decl->ident;
      assert(decl->inner == NULL);
      break;
    }
    case DECL_FUNCTION: {
      // Overwrite the dummy 0 we pushed with the actual return type
      decl->params.get[0] = tyid_to_uint32_t(current);

      // function cannot be const, restrict or volatile
      current = ty_intern(
          p->tyint, TY_FUNCTION, false, false, false, decl->params.get, (uint8_t)decl->params.n);
      vec_destroy(decl->params);
      break;
    }
    case DECL_POINTER: {
      uint32_t payload[] = {tyid_to_uint32_t(current)};
      current = ty_intern(
          p->tyint,
          TY_POINTER,
          decl->ptrqual.is_const,
          decl->ptrqual.is_restrict,
          decl->ptrqual.is_volatile,
          payload,
          1);
      break;
    }
    case DECL_INCOMPLETE_ARRAY: {
      uint32_t payload[] = {tyid_to_uint32_t(current)};
      current = ty_intern(p->tyint, TY_POINTER, false, false, false, payload, 1);
      break;
    }
    default:
      print_decl(p, decl);
      assert(false && "TODO");
    }
    decl = decl->inner;
  }

  *tyid = current;
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
      printf("IDENT(%s)", interner_fetch_str(p->interner, decl->ident));
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
