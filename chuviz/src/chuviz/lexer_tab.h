#ifndef LEXER_TAB_H_
#define LEXER_TAB_H_

#include "chucci_lex/token.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "thirdparty/termbox2.h"

typedef struct {
  SourceManager* sman;
  StringInterner* interner;
  vec(Token) tokens; // flattened TokenStream
  size_t selected; // selected token
  SrcID srcid;
} LexerTab;


LexerTab lexertab_init(SourceManager* sman, StringInterner* interner, SrcID srcid);
void lexertab_input(LexerTab* tab, struct tb_event* event);
void render_lexert(LexerTab* tab);


#endif
