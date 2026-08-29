#include "chucci_diag/cc_diag.h"
#include "chucci_lex/lexer.h"
#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "core/clexer_utils.h"
#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static InternID keyword_ids[_keyword_count];

TokenStream
lexer_new(SourceManager* sman, SrcScanner scanner, StringInterner* interner, jmp_buf* onerror) {
  Lexer* lexer = malloc(sizeof(Lexer));
  *lexer = (Lexer){0};
  lexer->sman = sman;
  lexer->scanner = scanner;
  lexer->interner = interner;
  lexer->engine = new_engine(cc_diaginfos, _cc_diaginfos_len, sman, onerror);

  // TODO: Relative caching (instead of kind, do (kind - first keyword), so that order doesnt
  // matter) (see token.h for the minor problem with current approach)

  // 0 id reserved as empty InternID
  // we only cache once
  if (keyword_ids[0] == 0) {
#define X(kind, str) keyword_ids[kind] = intern(strview(str), interner);
    KEYWORDS(X)
#undef X
  }

  return ts_from_func(lexer, lexer_next, lexer_peek, lexer_free);
}

static Token lex_op_sep(Lexer* l, int ch) {
  Span span = span_begin(&l->scanner);
#define X(kind, str, ch1)                                                                          \
  if (ch1 == ch && match_str(&l->scanner, str)) {                                                  \
    span_end(&span, &l->scanner);                                                                  \
    return token_new(span, kind);                                                                  \
  }
  OPERATORS(X)
  SEPARATORS(X)
#undef X
  __builtin_unreachable();
}

static int chucci_nextch(SrcScanner* scanner) {
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

static void skip_unwanted(DiagEngine* engine, SrcScanner* scanner) {
  uint32_t last_id;
  do {
    last_id = scanner->id;
    Span span = span_begin(scanner);
    int res = skip_c_comments(scanner);
    if (res < 0) {
      span_end(&span, scanner);
      throw_diag(engine, span, CC_LEX_INVALID_C_BLOCK_COMMENT);
    }
    skip_space(scanner);
  } while (scanner->id != last_id && peekch(scanner) != EOF);
}

static void skip_unwanted_except_newline(DiagEngine* engine, SrcScanner* scanner) {
  uint32_t last_id;
  do {
    last_id = scanner->id;
    Span span = span_begin(scanner);
    if (skip_c_comments(scanner) < 0) {
      span_end(&span, scanner);
      throw_diag(engine, span, CC_LEX_INVALID_C_BLOCK_COMMENT);
    }

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

static Token lex_ident(Lexer* l, int ch) {
  Span span = span_begin(&l->scanner);
  while (isalnum(ch) || ch == '_') {
    chucci_nextch(&l->scanner);
    ch = peekch(&l->scanner);
  }
  span_end(&span, &l->scanner);
  InternID id = intern(span_sv(l->sman, span), l->interner);

  // keyword checking
  // 'i' here is not only an index, but also the TokenKind
  // NOTE: you gotta change this if you change the logic to use relative ordering
  // instead of depending on the KEYWORDS being the first thing in the enum
  uint32_t i = _keyword_count;
  while (i--) {
    if (keyword_ids[i] == id)
      return token_new(span, i);
  }

  return token_new_ident(span, TOK_IDENT, id);
}

Token lexer_next(void* ctx) {
  Lexer* l = ctx;
  if (l->in_pp_directive)
    skip_unwanted_except_newline(&l->engine, &l->scanner);
  else
    skip_unwanted(&l->engine, &l->scanner);

  int ch = peekch(&l->scanner);

  if (ch == EOF)
    return EOF_TOKEN;

  if (ch == '#')
    l->in_pp_directive = true;

  if (ch == '\n') {
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
    span_end(&span, &l->scanner);
    if (res < 0)
      throw_diag(&l->engine, span, CC_LEX_INVALID_STRING);
    return token_new(span, TOK_STR);
  }

  throw_diag(&l->engine, span_begin(&l->scanner), CC_LEX_UNEXPECTED_CHAR, ch);
}

Token lexer_peek(void* ctx) {
  Lexer* lexer = ctx;
  Span checkpoint = span_begin(&lexer->scanner);
  Token token = lexer_next(ctx);
  scanner_rewind(&lexer->scanner, checkpoint);
  return token;
}

void lexer_free(void** lexer) {
  if (*lexer)
    free(*lexer);
  *lexer = NULL;
}
