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
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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

static ASTNode* make_ast_node(Parser* p, void* data, size_t len, ASTKind kind) {
  ASTNode* node = vmarena_alloc(p->arena, len + sizeof(ASTNode));
  node->kind = kind;
  memcpy(node->data, data, len);

  return node;
}

static Block parse_block(Parser* p) {
  (void)p;
  Block b = {0};
  return b;
}

static ASTNode* parse_func_def(Parser* p, InternID id, TypeID tyid) {
  printf("Parsing func def\n");
  Block block = parse_block(p);
  FuncDefNode node = {0};
  node.block = block;
  node.ident = id;
  node.type = tyid;

  return make_ast_node(p, &node, sizeof(FuncDefNode), AST_FUNC_DEF);
}

static ASTNode* parse_func_decl(Parser* p, InternID id, TypeID tyid) {

  FuncDeclNode node = {0};
  node.ident = id;
  node.type = tyid;

  return make_ast_node(p, &node, sizeof(FuncDeclNode), AST_FUNC_DECL);
}

static ASTNode* parse_func(Parser* p, InternID id, TypeID tyid) {
  Token token = ts_next(&p->ts);
  if (token.kind == SEP_LCURLY)
    return parse_func_def(p, id, tyid);
  else if (token.kind == SEP_SEMI)
    return parse_func_decl(p, id, tyid);

  assert(false && "Not possible");
  __builtin_unreachable();
}

ASTNode* parse_next(Parser* p) {
  TypeID tyid;
  StorageClass sc;
  parse_decl_specifier(p, &tyid, &sc);

  Declarator* decl = parse_declarator(p);
  print_decl(p, decl);

  InternID name = 0;
  unwind_declarator(&tyid, &name, decl, p, tyid);

  Type* type = ty_fetch(p->tyint, tyid);

  // Function decl or def...
  if (type->kind == TY_FUNCTION)
    return parse_func(p, name, tyid);

  return NULL;
}

void print_ast(Parser* p, ASTNode* ast) {
  switch (ast->kind) {
  case AST_FUNC_DECL: {
    FuncDeclNode node = *(FuncDeclNode*)&ast->data;
    printf("func_decl[%s](", interner_fetch_str(p->interner, node.ident));

    Type* type = ty_fetch(p->tyint, node.type);

    // Parameters start at index 1 and come in pairs (name, type)
    for (uint8_t i = 1; i < type->payload_len; i += 2) {
      if (i > 1) {
        printf(", ");
      }

      InternID param_name = *(InternID*)&type->payload[i];
      TypeID param_ty = *(TypeID*)&type->payload[i + 1];

      // Print param name (handle anonymous parameters like `int foo(int)`)
      if (param_name != 0) {
        printf("%s: %d", interner_fetch_str(p->interner, param_name), param_ty.id);
      } else {
        printf("<unnamed>: %d", param_ty.id);
      }
    }

    // Return type is at index 0
    TypeID ret_ty = *(TypeID*)&type->payload[0];
    printf(") -> %d\n", ret_ty.id);
    break;
  }

  case AST_FUNC_DEF: {
    FuncDefNode node = *(FuncDefNode*)&ast->data;
    printf("func_def[%s](", interner_fetch_str(p->interner, node.ident));

    Type* type = ty_fetch(p->tyint, node.type);

    // Parameters start at index 1 and come in pairs (name, type)
    for (uint8_t i = 1; i < type->payload_len; i += 2) {
      if (i > 1) {
        printf(", ");
      }

      InternID param_name = *(InternID*)&type->payload[i];
      TypeID param_ty = *(TypeID*)&type->payload[i + 1];

      if (param_name != 0) {
        printf("%s: %d", interner_fetch_str(p->interner, param_name), param_ty.id);
      } else {
        printf("<unnamed>: %d", param_ty.id);
      }
    }

    // Return type is at index 0
    TypeID ret_ty = *(TypeID*)&type->payload[0];
    printf(") -> %d\n", ret_ty.id);
    break;
  }

  default:
    assert(false && "NOT IMPLEMENTED");
  }
}

VarDeclNode parse_var_decl(Parser* p) {
  TypeID tyid;
  StorageClass sc;
  parse_decl_specifier(p, &tyid, &sc);
  Declarator* decl = parse_declarator(p);

  VarDeclNode node = {0};
  unwind_declarator(&node.type, &node.ident, decl, p, tyid);

  return node;
}
