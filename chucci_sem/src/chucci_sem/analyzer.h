#ifndef ANALYZER_H_
#define ANALYZER_H_

#include "chucci_parse/parser.h"
#include "chucci_sem/symbol.h"
#include "core/vmem_arena.h"

typedef struct {
  VMEMArena* arena;
  SymbolTable* syms;  
} SemAnalyzer;

SemAnalyzer new_
void sem_analyze(SemAnalyzer* analyzer, ASTNode ast);

#endif
