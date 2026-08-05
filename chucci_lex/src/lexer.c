#include "chucci_lex/lexer.h"
#include "chucci_lex/token.h"
#include "core/clexer_utils.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

static InternID keyword_ids[__keyword_count];

Lexer lexer_new(SourceManager* sman, SrcID srcid, StringInterner* interner) {
  Lexer lexer = {0};
  lexer.sman = sman;
  lexer.scanner = scanner_new(sman, srcid);
  lexer.interner = interner;

// TODO: Relative caching (instead of kind, do (kind - first keyword), so that order doesnt matter)
// (see token.h for the minor problem with current approach)
#define X(kind, str) keyword_ids[kind] = intern(strview(str), interner);
  KEYWORDS(X)
#undef X

  return lexer;
}

Token lex_op_sep(Lexer* lexer, int ch) {
  Span span = span_begin(&lexer->scanner);
#define X(kind, str, ch1)                                                                          \
  if (ch1 == ch && match_str(&lexer->scanner, str)) {                                              \
    span_end(&span, &lexer->scanner);                                                              \
    return token_new(span, kind);                                                                  \
  }
  OPERATORS(X)
  SEPARATORS(X)
#undef X
  assert(false);
}

static int chucci_nextch(SrcScanner* scanner) {
  int ch = nextch(scanner);
  if (ch == '\\') {
    int lookahead = peekch(scanner);
    // unix \n
    if (lookahead == '\n') {
      nextch(scanner);
      return chucci_nextch(scanner);
    }
    // windows \r\n
    if (lookahead == '\r') {
      nextch(scanner);
      if (peekch(scanner) == '\n')
        nextch(scanner);
      return chucci_nextch(scanner);
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

Token lex_ident(Lexer* lexer, int ch) {
  Span span = span_begin(&lexer->scanner);
  while (isalnum(ch) || ch == '_') {
    nextch(&lexer->scanner);
    ch = peekch(&lexer->scanner);
  }
  span_end(&span, &lexer->scanner);
  InternID id = intern(span_sv(lexer->sman, span), lexer->interner);

  // keyword checking
  // 'i' here is not only an index, but also the
  // NOTE: you gotta change this if you change the logic to use relative ordering
  // instead of depending on the KEYWORDS being the first thing in the enum
  size_t i = __keyword_count;
  while (i--) {
    if (keyword_ids[i] == id)
      return token_new(span, i);
  }

  return token_new_ident(span, TOK_IDENT, id);
}

Token lexer_next(Lexer* lexer) {
  skip_unwanted(&lexer->scanner);
  int ch = peekch(&lexer->scanner);

  if (ch == EOF)
    return EOF_TOKEN;

  if (isdigit(ch)) {
    Span span = span_begin(&lexer->scanner);
    while (isalnum(ch) || ch == '.') {
      nextch(&lexer->scanner);
      ch = peekch(&lexer->scanner);
    }
    span_end(&span, &lexer->scanner); // FIX: Added missing span_end!
    return token_new(span, TOK_VAL);
  }

  if (isalpha(ch) || ch == '_')
    return lex_ident(lexer, ch);

  if (ch == '<') {
    Span span = span_begin(&lexer->scanner);
    nextch(&lexer->scanner); // <

    // try to lex the <....> string
    while (ch != '\n' && ch != '>') {
      nextch(&lexer->scanner);
      ch = peekch(&lexer->scanner);
    }

    if (ch == '>') {
      span_end(&span, &lexer->scanner);
      nextch(&lexer->scanner); // >
      span.offset++;           // skip the first <
      span.len--;
      return token_new(span, TOK_ANGLE);
    }

    // rewind if failed
    scanner_rewind(&lexer->scanner, span);
    ch = peekch(&lexer->scanner);
  }

  if (is_op(ch) || is_sep(ch))
    return lex_op_sep(lexer, ch);

  if (ch == '\"') {
    Span span = span_begin(&lexer->scanner);
    int res = lex_cstr(&lexer->scanner);
    if (res < 0)
      assert(false);
    span_end(&span, &lexer->scanner);
    return token_new(span, TOK_STR);
  }

  if (ch == '\'') {
    Span span = span_begin(&lexer->scanner);

    nextch(&lexer->scanner); // '\''

    ch = peekch(&lexer->scanner);
    if (ch == '\\') {
      nextch(&lexer->scanner); // '\'
      nextch(&lexer->scanner); // escaped character
    } else if (ch != '\'')
      nextch(&lexer->scanner); // normal character

    assert(peekch(&lexer->scanner) == '\'');

    nextch(&lexer->scanner); // Consume the closing '\''

    span_end(&span, &lexer->scanner);
    return token_new(span, TOK_VAL);
  }

  assert(false);
}
