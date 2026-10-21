#include "core/diagnostics.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "lexer/token.h"
#include "lexer/token_stream.h"
#include "parser/expr.h"
#include "parser/parser.h"
#include "parser/scope.h"
#include "parser/stmt.h"
#include "parser/topdecl.h"
#include "parser/type.h"
#include <stdio.h>

static TopDecl parse_top_decl(Parser* p) {
  // TODO: Actual good C type parsing (parse_decl_spec, parse_decl, pratt)
  // TODO: Other top level declarations
  // TODO: Function as type
  // TODO: Function declaration support
  //
  // Functions
  VoleType fntype = (VoleType){.kind = TK_FUNCTION};
  FuncDef def = {0};
  def.ret = parse_type(p);
  def.name = ts_expect(&p->ts, TOK_IDENT, p->engine).ident;

  ts_expect(&p->ts, SEP_LPAREN, p->engine);
  ts_expect(&p->ts, SEP_RPAREN, p->engine);

  register_var(&p->globals, fntype, def.name);
  ts_expect(&p->ts, SEP_LCURLY, p->engine);

  Token tok = ts_peek(&p->ts);
  while (tok.kind != SEP_RCURLY) {
    Stmt stmt = parse_stmt(p);
    vec_push(def.stmts, stmt);
    tok = ts_peek(&p->ts);
  }
  ts_expect(&p->ts, SEP_RCURLY, p->engine);

  TopDecl decl = {0};
  decl.kind = TD_FUNC;
  decl.fndef = def;

  return decl;
}

static void print_expr(Expr expr) {
  switch (expr.kind) {
  case EXPR_NUM:
    printf("%d", expr.num);
    break;
  }
}

static void print_stmt(Stmt stmt) {
  switch (stmt.kind) {
  case STMT_RETURN:
    printf("return ");
    print_expr(stmt.ret_expr);
  }
  printf("; ");
}

static void print_td(Parser* p, TopDecl td) {
  switch (td.kind) {
  case TD_FUNC:
    printf("fn %s() { ", interner_fetch_str(p->interner, td.fndef.name));
    for (uint32_t i = 0; i < td.fndef.stmts.n; i++)
      print_stmt(td.fndef.stmts.get[i]);
    printf("}\n");
  }
}

Parser parse(TokenStream ts, VMEMArena* arena, DiagEngine* engine, StringInterner* interner) {
  Token tok = ts_peek(&ts);
  Parser p = {0};
  p.interner = interner;
  p.ts = ts;
  p.engine = engine;
  p.arena = arena;

  while (tok.kind != TOK_EOF) {
    TopDecl decl = parse_top_decl(&p);
    print_td(&p, decl);
    tok = ts_peek(&p.ts);
  }
  return p;
}
