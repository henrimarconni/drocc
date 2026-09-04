/**
  THE CHUCCI PARSER

  This Parser uses 3 different categories of ast nodes:
  1) Top Level Declarations (ASTNode)
  2) Statement (Stmt)
  3) Expression (Expr)

  Here:
  1) ASTNode can be: VarDeclNode, FuncDeclNode, VarDefNode (but RHS must be a constant value), FuncDefNode,
  TypeDefNode (any type of definition, can be struct or enum or union or typedefs)

  2) Stmt can be: VarDefNode, VarDeclNode, FuncDeclNode, Expr.

  3) Expr can be: BinOpNode, UnaryOpNode, FuncCallNode, AssignmentNode

  whenever parse_next() is called, it parses a single Top Level Declaration and returns ASTNode*.

  Here, Declarator is a tool used to parse types.
  Types are stored using a faster, data oriented approach, instead of pointers, we have payload struct
  with each element containing data in a certain pattern as calculated by the TypeKind:

  for eg:

  TY_FUNCTION: payload[] = return type id, param1 intern id, param1 type id, param2 internid, param2 typeid...... (internid = 0 if not specified)
  TY_POINTER: payload = target type id
  TY_INCOMPLETE_ARRAY: payload = target type id
*/

#ifndef PARSER_H_
#define PARSER_H_

#include "chucci_lex/token_stream.h"
#include "chucci_parse/scope.h"
#include "chucci_parse/type.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/slice.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <stdint.h>


typedef enum {
  AST_VAR_DECL,
  AST_FUNC_DECL,
  AST_VAR_DEF,
  AST_FUNC_DEF,
  AST_TYPE_ADEF
} ASTKind;

typedef struct {
  int i; // TODO
} Expr;

typedef struct {
  int i; // TODO
} Stmt;

typedef struct {
  ScopeID scope;
  slice(Stmt) stmts;
} Block;

typedef struct {
  TypeID type;
  InternID ident;
} VarDeclNode;

typedef struct {
  TypeID type;
  InternID ident;
} FuncDeclNode;

typedef struct {
  TypeID type;
  InternID ident;
  Expr val;
} VarDefNode;

typedef struct {
  TypeID type;
  InternID ident;
  Block block;
} FuncDefNode;

typedef struct {
  ASTKind kind;
  uint8_t data[];
} ASTNode;

typedef struct Parser{
  TokenStream ts;
  SourceManager* sman;
  vec(LocalScope) scopes;
  TypeInterner* tyint;
  StringInterner* interner;
  VMEMArena* arena;
  VMEMArena* scratch;
} Parser;

Parser parser_new(TokenStream ts, SourceManager* sman, StringInterner* interner, VMEMArena* arena);
ASTNode* parse_next(Parser* p);
VarDeclNode parse_var_decl(Parser* p);
void print_ast(Parser* p, ASTNode* ast);


#endif
