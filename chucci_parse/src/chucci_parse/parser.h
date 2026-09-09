/**
  THE CHUCCI PARSER

  This Parser uses 3 different categories of ast nodes:
  1) Top Level Declarations (ASTNode)
  2) Statement (Stmt)
  3) Expression (Expr)

  Here:
  1) ASTNode can be: VarDeclNode, FuncDeclNode, VarDefNode (but RHS must be a constant value),
  FuncDefNode, TypeDefNode (any type of definition, can be struct or enum or union or typedefs)

  2) Stmt can be: VarDefNode, VarDeclNode, FuncDeclNode, Expr.

  3) Expr can be: BinOpNode, UnaryOpNode, FuncCallNode, AssignmentNode

  whenever parse_next() is called, it parses a single Top Level Declaration and returns ASTNode.

  Here, Declarator is a tool used to parse types.
  Types are stored using a faster, data oriented approach, instead of pointers, we have payload
  struct with each element containing data in a certain pattern as calculated by the TypeKind:

  for eg:

  TY_FUNCTION: payload[] = return type id, param1 intern id, param1 type id, param2 internid, param2
  typeid...... (internid = 0 if not specified) TY_POINTER: payload = target type id
  TY_INCOMPLETE_ARRAY: payload = target type id
*/

#ifndef PARSER_H_
#define PARSER_H_

#include "chucci_lex/token_stream.h"
#include "chucci_parse/type.h"
#include "chucci_parse/stmt.h"
#include "chucci_parse/typeinterner.h"
#include "core/slice.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include <stdint.h>

extern bool is_unary[_token_kind_count];
extern bool is_binary[_token_kind_count];

typedef struct {
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
  vmptr(Expr) val;
} VarDefNode;

typedef struct {
  TypeID type;
  InternID ident;
  Block block;
} FuncDefNode;

typedef enum { AST_VAR_DECL, AST_FUNC_DECL, AST_VAR_DEF, AST_FUNC_DEF } ASTKind;

typedef struct {
  ASTKind kind;
  vmptr(void) data;
} ASTNode;

typedef struct Parser {
  TokenStream ts;
  SourceManager* sman;
  TypeInterner* tyint;
  StringInterner* interner;
  VMEMArena* arena;
  // Arena specifically for parser related stuff
  // This can be reset after every parser_next to save space
  VMEMArena* parena;
} Parser;

Parser parser_new(TokenStream ts, SourceManager* sman, StringInterner* interner, VMEMArena* arena);
ASTNode parse_next(Parser* p);
VarDeclNode parse_var_decl(Parser* p);
void print_ast(Parser* p, ASTNode ast);

#endif
