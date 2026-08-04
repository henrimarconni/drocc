#ifndef CODEGEN_H
#define CODEGEN_H

#include "saulgood/parser.h"
#include "core/stringbuilder.h"


typedef struct {
  StringBuilder output;
  size_t test_len;
} SGCodegen;

void generate_code(SGCodegen* c, ParserState* state);
void codegen_destroy(SGCodegen* c);

#endif
