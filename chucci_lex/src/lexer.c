#include "chucci_lex/lexer.h"
#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "core/clexer_utils.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static InternID keyword_ids[_keyword_count];

TokenStream lexer_new(SourceManager* sman, SrcID srcid, StringInterner* interner) {
  Lexer* lexer = malloc(sizeof(Lexer));
  *lexer = (Lexer){0};
  lexer->sman = sman;
  lexer->scanner = scanner_new(sman, srcid);
  lexer->interner = interner;

// TODO: Relative caching (instead of kind, do (kind - first keyword), so that order doesnt matter)
// (see token.h for the minor problem with current approach)
#define X(kind, str) keyword_ids[kind] = intern(strview(str), interner);
  KEYWORDS(X)
#undef X

  return ts_from_func(lexer, lexer_next, lexer_peek, lexer_free);
}

Token lex_op_sep(Lexer* l, int ch) {
  Span span = span_begin(&l->scanner);
#define X(kind, str, ch1)                                                                          \
  if (ch1 == ch && match_str(&l->scanner, str)) {                                                  \
    span_end(&span, &l->scanner);                                                                  \
    return token_new(span, kind);                                                                  \
  }
  OPERATORS(X)
  SEPARATORS(X)
#undef X
  assert(false);
}

int chucci_nextch(SrcScanner* scanner) {
  int ch = nextch(scanner);
  if (ch == '\\') {
    int lookahead = peekch(scanner);
    // unix \n
    if (lookahead == '\n') {
      nextch(scanner);
      return nextch(scanner);
    }
    // windows \r\n
    if (lookahead == '\r') {
      nextch(scanner);
      if (peekch(scanner) == '\n')
        nextch(scanner);
      return nextch(scanner);
    }
  }
  return ch;
}

void skip_unwanted(SrcScanner* scanner) {
  int last_id;
  do {
    last_id = scanner->id;
    int res = skip_c_comments(scanner);
    if (res < 0)
      assert(false);
    skip_space(scanner);
  } while (scanner->id != last_id && peekch(scanner) != EOF);
}

void skip_unwanted_except_newline(SrcScanner* scanner) {
  int last_id;
  do {
    last_id = scanner->id;
    if (skip_c_comments(scanner) < 0)
      assert(false);

    int ch;
    while ((ch = peekch(scanner)) != EOF) {
      if (isspace(ch) && ch != '\n')
        nextch(scanner);
      else if (ch == '\\') {
        int lookahead = peeknextch(scanner);
        if (lookahead == '\n') {
          nextch(scanner);
          nextch(scanner);
        } else if (lookahead == '\r') {
          nextch(scanner);
          nextch(scanner);
          if (peekch(scanner) == '\n')
            nextch(scanner);
        } else
          break;
      } else
        break;
    }
  } while (scanner->id != last_id && peekch(scanner) != EOF);
}

Token lex_ident(Lexer* l, int ch) {
  Span span = span_begin(&l->scanner);
  while (isalnum(ch) || ch == '_') {
    chucci_nextch(&l->scanner);
    ch = peekch(&l->scanner);
  }
  span_end(&span, &l->scanner);
  InternID id = intern(span_sv(l->sman, span), l->interner);

  // keyword checking
  // 'i' here is not only an index, but also the
  // NOTE: you gotta change this if you change the logic to use relative ordering
  // instead of depending on the KEYWORDS being the first thing in the enum
  size_t i = _keyword_count;
  while (i--) {
    if (keyword_ids[i] == id)
      return token_new(span, i);
  }

  return token_new_ident(span, TOK_IDENT, id);
}

Token lexer_next(void* ctx) {
  Lexer* l = ctx;
  if (l->in_pp_directive)
    skip_unwanted_except_newline(&l->scanner);
  else
    skip_unwanted(&l->scanner);

  int ch = peekch(&l->scanner);

  if (ch == EOF)
    return EOF_TOKEN;

  if (ch == '#')
    l->in_pp_directive = true;

  if (ch == '\n') {
    assert(l->in_pp_directive);
    l->in_pp_directive = false;
    Span span = span_begin(&l->scanner);
    chucci_nextch(&l->scanner);
    span_end(&span, &l->scanner);
    return token_new(span, SEP_NEWLINE);
  }

  if (isdigit(ch)) {
    Span span = span_begin(&l->scanner);
    while (isalnum(ch) || ch == '.') {
      chucci_nextch(&l->scanner);
      ch = peekch(&l->scanner);
    }
    span_end(&span, &l->scanner); // FIX: Added missing span_end!
    return token_new(span, TOK_VAL);
  }

  if (isalpha(ch) || ch == '_')
    return lex_ident(l, ch);

  if (ch == '<') {
    Span span = span_begin(&l->scanner);
    chucci_nextch(&l->scanner); // <

    // try to lex the <....> string
    while (ch != '\n' && ch != '>') {
      chucci_nextch(&l->scanner);
      ch = peekch(&l->scanner);
    }

    if (ch == '>') {
      span_end(&span, &l->scanner);
      chucci_nextch(&l->scanner); // >
      span.offset++;              // skip the first <
      span.len--;
      return token_new(span, TOK_ANGLE);
    }

    // rewind if failed
    scanner_rewind(&l->scanner, span);
    ch = peekch(&l->scanner);
  }

  if (is_op(ch) || is_sep(ch))
    return lex_op_sep(l, ch);

  if (ch == '\"') {
    Span span = span_begin(&l->scanner);
    int res = lex_cstr(&l->scanner);
    if (res < 0)
      assert(false);
    span_end(&span, &l->scanner);
    return token_new(span, TOK_STR);
  }

  if (ch == '\'') {
    Span span = span_begin(&l->scanner);

    chucci_nextch(&l->scanner); // '\''

    ch = peekch(&l->scanner);
    if (ch == '\\') {
      chucci_nextch(&l->scanner); // '\'
      chucci_nextch(&l->scanner); // escaped character
    } else if (ch != '\'')
      chucci_nextch(&l->scanner); // normal character

    assert(peekch(&l->scanner) == '\'');

    chucci_nextch(&l->scanner); // Consume the closing '\''

    span_end(&span, &l->scanner);
    return token_new(span, TOK_VAL);
  }

  highlight_span(l->sman, span_begin(&l->scanner));
  fflush(stdout);

  assert(false);
}

Token lexer_peek(void* ctx) {
  Lexer* lexer = ctx;
  Span checkpoint = span_begin(&lexer->scanner);
  Token token = lexer_next(ctx);
  scanner_rewind(&lexer->scanner, checkpoint);
  return token;
}

void lexer_free(void* lexer) { free(lexer); }
