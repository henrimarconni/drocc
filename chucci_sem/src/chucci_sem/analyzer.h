#ifndef ANALYZER_H_
#define ANALYZER_H_

#include "chucci_parse/parser.h"
#include "chucci_sem/symbol.h"

typedef struct {
  Parser* p;
  SymbolTable* syms;  
} SemAnalyzer;

SemAnalyzer new_analyzer(Parser* parser, SymbolTable* table);
void sem_analyze(SemAnalyzer* analyzer, ASTNode ast);

#endif
