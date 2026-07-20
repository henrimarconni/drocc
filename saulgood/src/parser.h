#ifndef PARSER_H
#define PARSER_H

#include "span.h"
#include "vec.h"
#include "vmem_arena.h"
#include "stringdef.h"

typedef vec(bstr) InputFiles;


typedef struct {
  Span group;
  Span name;
  Span desc;
  Span body;
} Test;

typedef struct {
  vec(Test) tests;
  ostr source;
  Span c_code;
} TestFile;

typedef struct {
  vec(TestFile) files;
  VMEMArena* arena;
} ParserState;


void parse_file(ParserState* state, bstr filename);


#endif
