#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include "vec.h"

typedef vec(char) StringBuilder;

typedef struct {
  Parser* parser;
  StringBuilder output;
} Codegen;

void generate_code(Codegen* c, Parser* p);
void codegen_destroy(Codegen* c);


#endif
