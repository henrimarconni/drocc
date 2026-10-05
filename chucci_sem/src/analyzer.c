#include "chucci_parse/parser.h"
#include "chucci_parse/stmt.h"
#include "chucci_parse/typeinterner.h"
#include "chucci_sem/analyzer.h"
#include "chucci_sem/symbol.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>

SemAnalyzer new_analyzer(Parser* parser, SymbolTable* table) {
  SemAnalyzer analyzer = {0};
  analyzer.syms = table;
  analyzer.p = parser;

  return analyzer;
}

/// TODO: ANALYZE THIS AFTER USER DEFINED TYPES ARE IMPLEMENTED
static void expect_sym_type(SemAnalyzer* a, TypeID tyid, InternID id) {
  (void)a;
  (void)tyid;
  (void)id;
}

static void analyze_fn_type(SemAnalyzer* a, TypeID type) {
  (void)a;
  (void)type;
}

static void analyze_expr(SemAnalyzer* a, Expr expr) {
  (void)a;
  (void)expr;
}

static void analyze_stmt(SemAnalyzer* a, Stmt stmt) {
  switch (stmt.kind) {
  case STMT_RETURN:
    StmtReturn* ret = vmderef(a->p->arena, stmt.data);
    analyze_expr(a, ret->expr);
  }
}

static void analyze_block(SemAnalyzer* a, Block b) {
  for (uint32_t i = 0; i < b.stmts.n; i++) {
    Stmt stmt = b.stmts.get[i];
    analyze_stmt(a, stmt);
  }
}

void sem_analyze(SemAnalyzer* a, ASTNode ast) {
  switch (ast.kind) {
  case AST_FUNC_DECL: {
    FuncDeclNode* node = vmderef(a->p->parena, ast.data);
    analyze_fn_type(a, node->type);
    expect_sym_type(a, node->type, node->ident);
    break;
  }
  case AST_FUNC_DEF: {
    FuncDefNode* node = vmderef(a->p->parena, ast.data);
    analyze_fn_type(a, node->type);
    expect_sym_type(a, node->type, node->ident);
    analyze_block(a, node->block);
    break;
  }
  default:
    assert(false && "Not Implemented");
  }
}
