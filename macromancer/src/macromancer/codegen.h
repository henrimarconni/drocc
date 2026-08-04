#ifndef CODEGEN_H
#define CODEGEN_H

#include "core/srcman.h"
#include "macromancer/parser.h"
#include "core/stringbuilder.h"

typedef struct {
  MMParser* parser;
  StringBuilder output;
  SourceManager* sman;
} MMCodegen;

void generate_code(MMCodegen* c, MMParser* p);
void codegen_destroy(MMCodegen* c);


#endif
