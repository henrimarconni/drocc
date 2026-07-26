#ifndef CODEGEN_H
#define CODEGEN_H

#include "saulgood/parser.h"
#include "core/stringbuilder.h"


typedef struct {
  StringBuilder output;
  size_t test_len;
} Codegen;

void generate_code(Codegen* c, ParserState* state);
void codegen_destroy(Codegen* c);

#endif
