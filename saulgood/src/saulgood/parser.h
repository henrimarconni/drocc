#ifndef PARSER_H
#define PARSER_H

#include "core/diagnostics.h"
#include "core/srcman.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "core/stringdef.h"
#include <setjmp.h>

typedef vec(bstr) InputFiles;


typedef enum {
  TOK_BLOCK, // full C code block is considered a single token
  TOK_STR, // for test descriptions.
  TOK_ID,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_COLON,
} SGTokenType;

typedef struct {
  SGTokenType type;
  StringView sv;
  Span span;
} SGToken;

typedef struct {
  SGToken group;
  SGToken name;
  SGToken desc;
  SGToken body;
} Test;

typedef enum {
  CG_TEST,
  CG_CBLOCK,
} CGenNodeType;

typedef struct {
  union {
    Test test;
    SGToken c_code;
  };
  CGenNodeType type;
} SGCodegenNode;

typedef struct {
  vec(SGCodegenNode) nodes;
  SrcScanner scanner;
} TestFile;

typedef struct {
  vec(TestFile) files;
  DiagEngine engine;
  VMEMArena* arena;
} ParserState;


void parse_files(ParserState* state, SourceManager* sman, InputFiles files, jmp_buf* onerror);
void parser_free(ParserState* state);


#endif
