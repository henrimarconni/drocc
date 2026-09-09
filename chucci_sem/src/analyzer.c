#include "chucci_parse/parser.h"
#include "chucci_sem/analyzer.h"
#include "chucci_sem/symbol.h"

SemAnalyzer new_analyzer(VMEMArena* arena, SymbolTable* table) {
  SemAnalyzer analyzer = {0};
  analyzer.arena = arena;
  analyzer.syms = table;

  return analyzer;
}

void sem_analyze(SemAnalyzer* a, ASTNode ast) {
  switch (ast.kind) {
  case AST_FUNC_DECL: {
  }
  }
}
