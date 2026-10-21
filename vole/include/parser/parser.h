#ifndef PARSER_H_
#define PARSER_H_

#include "core/diagnostics.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include "lexer/token_stream.h"
#include "parser/scope.h"
#include "parser/topdecl.h"

typedef struct Parser {
  TokenStream ts;
  VMEMArena* arena;
  DiagEngine* engine;
  StringInterner* interner;

  vec(TopDecl) top_decls;
  Scope globals;
} Parser;

Parser parse(TokenStream ts, VMEMArena* arena, DiagEngine* engine, StringInterner* interner);

#endif
