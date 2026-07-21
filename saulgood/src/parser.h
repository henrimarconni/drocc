#ifndef PARSER_H
#define PARSER_H

#include "diagnostics.h"
#include "scanner.h"
#include "span.h"
#include "vec.h"
#include "vmem_arena.h"
#include "stringdef.h"
#include <setjmp.h>

typedef vec(bstr) InputFiles;


typedef enum {
  TOK_BLOCK, // full C code block is considered a single token
  TOK_STR, // for test descriptions.
  TOK_ID,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_COLON,
} TokenType;

typedef struct {
  TokenType type;
  Span span;
} Token;

typedef struct {
  Span group;
  Span name;
  Span desc;
  Span body;
} Test;

typedef enum {
  CG_TEST,
  CG_CBLOCK,
} CGenNodeType;

typedef struct {
  union {
    Test test;
    Span c_code;
  };
  CGenNodeType type;
} CodegenNode;

typedef struct {
  vec(CodegenNode) nodes;
  SourceFile source;
} TestFile;

typedef struct {
  vec(TestFile) files;
  DiagEngine engine;
  VMEMArena* arena;
} ParserState;


void parse_files(ParserState* state, InputFiles files, jmp_buf* onerror);


#endif
