#ifndef PREPROC_H_
#define PREPROC_H_

#include "chucci_lex/token_stream.h"
#include "core/slice.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"

typedef slice(bstr) PPSearchPaths;

typedef struct {
  TokenStreamStack stack;
  VMEMArena* arena;
  SourceManager* sman;
  StringInterner* interner;

  PPSearchPaths sys_search;
  PPSearchPaths search;
} Preprocessor;

#define PREPROC_CMDS(X)\
X(PP_INCLUDE, "include")\
X(PP_DEFINE, "define")\
X(PP_IFNDEF, "ifndef")\
X(PP_IFDEF, "ifdef")\
X(PP_IF, "if")\
X(PP_ELIF, "elif")\
X(PP_ELSE, "else")

typedef enum {
#define X(a, b) a,
PREPROC_CMDS(X)
#undef X
_preproc_cmd_count
} PreprocCMD;

TokenStream preproc_new(TokenStream ts, SourceManager* sman, StringInterner* interner, PPSearchPaths sys_search, PPSearchPaths search);
Token preproc_next(void* preproc);
Token preproc_peek(void* preproc);
void preproc_free(void** preproc);

#endif
