#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include "stringbuilder.h"


typedef struct {
  StringBuilder output;
} Codegen;

void generate_code(Codegen* c, ParserState* state);
void codegen_destroy(Codegen* c);

#endif
