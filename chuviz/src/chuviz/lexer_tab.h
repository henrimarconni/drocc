#ifndef LEXER_TAB_H_
#define LEXER_TAB_H_

#include "chucci_lex/token.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include "libterm/libterm.h"
#include <stddef.h>

typedef struct {
  SourceManager* sman;
  StringInterner* interner;
  TokenVec tokens; // flattened TokenStream
  VMEMArena* arena;

  size_t selected; // selected token
  SrcID srcid;
  int percentage1;
  int scroll_y;
} LexerTab;


LexerTab* lexertab_init(bstr file);
void lexertab_free(LexerTab* tab);
void lexertab_input(LexerTab* tab, struct lt_event* event);
void render_lexert(LexerTab* tab);


#endif
