#ifndef LEXER_TAB_H_
#define LEXER_TAB_H_

#include "chucci_lex/token.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "libterm/libterm.h"
#include <stddef.h>

typedef struct {
  SourceManager* sman;
  StringInterner* interner;
  vec(Token) tokens; // flattened TokenStream
  size_t selected; // selected token
  SrcID srcid;
  int percentage1;
  size_t scroll_y;
} LexerTab;


LexerTab lexertab_init(SourceManager* sman, StringInterner* interner, SrcID srcid);
void lexertab_input(LexerTab* tab, struct lt_event* event);
void render_lexert(LexerTab* tab);


#endif
