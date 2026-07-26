#ifndef CODEGEN_H
#define CODEGEN_H

#include "macromancer/parser.h"
#include "core/stringbuilder.h"

typedef struct {
  Parser* parser;
  StringBuilder output;
} Codegen;

void generate_code(Codegen* c, Parser* p);
void codegen_destroy(Codegen* c);


#endif
