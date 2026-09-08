#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_parse/decl_spec.h"
#include "chucci_parse/declarator.h"
#include "chucci_parse/expr.h"
#include "chucci_parse/parser.h"
#include "chucci_parse/stmt.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

bool is_unary[_token_kind_count] = {0};
bool is_binary[_token_kind_count] = {0};

Parser parser_new(TokenStream ts, SourceManager* sman, StringInterner* interner, VMEMArena* arena) {
  Parser p = {0};
  p.arena = arena;
  p.sman = sman;
  p.ts = ts;
  p.interner = interner;
  p.tyint = ty_interner_new();
  p.parena = vmarena_new(128 * 1024);

  if (!is_unary[OP_NOT]) {
#define X(kind, _) is_unary[kind] = true;
    UNARY_OPS(X)
#undef X
  }

  if (!is_binary[OP_NOT]) {
#define X(kind, _) is_binary[kind] = true;
    BINARY_OPS(X)
#undef X
  }

  return p;
}

static ASTNode* make_ast_node(Parser* p, void* data, size_t len, ASTKind kind) {
  ASTNode* node = vmarena_alloc(p->arena, len + sizeof(ASTNode));
  node->kind = kind;
  memcpy(node->data, data, len);

  return node;
}

static Block parse_block(Parser* p) {
  Block block = {0};
  vec(Stmt*) stmts = {0};

  Token token = ts_peek(&p->ts);
  while (token.kind != SEP_RCURLY) {
    vec_push(stmts, parse_stmt(p));
    token = ts_peek(&p->ts);
  }

  block.stmts.n = stmts.n;
  block.stmts.get = vmarena_alloc(p->arena, sizeof(Stmt*) * stmts.n);
  memcpy(block.stmts.get, stmts.get, sizeof(Stmt*) * stmts.n);
  vec_destroy(stmts);

  Token rcurly = ts_next(&p->ts);
  assert(rcurly.kind == SEP_RCURLY);

  return block;
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
    printf("func_decl[%s]", interner_fetch_str(p->interner, node.ident));
    Type* type = ty_fetch(p->tyint, node.type);

    printf("func_def[%s](", interner_fetch_str(p->interner, node.ident));
    print_func_type(p, type);
    puts("");
    break;
  }

  case AST_FUNC_DEF: {
    FuncDefNode node = *(FuncDefNode*)&ast->data;
    Type* type = ty_fetch(p->tyint, node.type);

    printf("func_def[%s]", interner_fetch_str(p->interner, node.ident));
    print_func_type(p, type);
    puts(" {");

    for (size_t i = 0; i < node.block.stmts.n; i++) {
      printf("    ");
      print_stmt(p, node.block.stmts.get[i]);
    }

    puts("}");
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
