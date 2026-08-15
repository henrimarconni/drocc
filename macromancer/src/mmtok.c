#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include "macromancer/mm_diag.h"
#include "macromancer/mmtok.h"
#include "macromancer/parser.h"
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

static void skip_comment(SrcScanner* scanner) {
  if (peekch(scanner) == '#') {
    while (peekch(scanner) != '\n' && peekch(scanner) != EOF) {
      nextch(scanner);
    }
  }
}

void skip_unwanted(SrcScanner* scanner) {
  uint32_t last_id;
  do {
    last_id = scanner->id;
    skip_comment(scanner);
    skip_space(scanner);
  } while (scanner->id != last_id && peekch(scanner) != EOF);
}

static MMToken tok_ident(MMParser* p) {
  Span span = span_begin(&p->scanner);

  nextch(&p->scanner);
  int ch = peekch(&p->scanner);
  while (ch != EOF && (isalnum((unsigned char)ch) || ch == '_')) {
    nextch(&p->scanner);
    ch = peekch(&p->scanner);
  }

  span_end(&span, &p->scanner);

  MMToken tok = {0};
  tok.type = MMT_IDENT;
  tok.span = span;
  tok.sv = span_sv(p->sman, span);

  if (memcmp(tok.sv.str, "as", 2) == 0)
    tok.type = MMT_AS;

  return tok;
}

static MMToken tok_surround(MMParser* p, char start, char end) {
  Span span = span_begin(&p->scanner);
  int ch = nextch(&p->scanner); // start character

  // we dont need to throw because tok_surround is always called
  // after checking if first character is `start`
  // @see tok_angstr and tok_str
  assert(ch == start);

  ch = peekch(&p->scanner);
  while (ch != end && ch != EOF) {
    nextch(&p->scanner);
    ch = peekch(&p->scanner);
  }

  nextch(&p->scanner);
  span_end(&span, &p->scanner);

  if (ch != end)
    throw_diag(&p->engine, span, MM_ERR_UNEXPECTED_EOF);

  MMToken tok = {0};
  tok.span = span;
  tok.sv = span_sv(p->sman, span);

  tok.sv.len -= 2;
  tok.sv.str++;

  return tok;
}

static MMToken tok_angstr(MMParser* p) {
  MMToken tok = tok_surround(p, '<', '>');
  tok.type = MMT_ANGSTR;
  return tok;
}

static MMToken tok_str(MMParser* p) {
  MMToken tok = tok_surround(p, '"', '"');
  tok.type = MMT_STR;
  return tok;
}

static MMToken tok_keyw(MMParser* p) {
  Span span = span_begin(&p->scanner);
  nextch(&p->scanner); // '$'
  int ch = peekch(&p->scanner);

  while (ch != EOF && (isalnum((unsigned char)ch) || ch == '_')) {
    nextch(&p->scanner);
    ch = peekch(&p->scanner);
  }

  span_end(&span, &p->scanner);

  MMToken tok = {0};
  tok.type = MMT_KEYW;
  tok.span = span;
  tok.sv = span_sv(p->sman, span);

  return tok;
}

static MMToken tok_simple(MMParser* p, MMTokenType type) {
  Span span = span_begin(&p->scanner);
  nextch(&p->scanner);
  span_end(&span, &p->scanner);

  return (MMToken){
      .type = type,
      .span = span,
      .sv = span_sv(p->sman, span),
  };
}

MMToken get_mmtok(MMParser* p) {
  skip_unwanted(&p->scanner);

  int ch = peekch(&p->scanner);
  if (ch == EOF)
    throw_diag(&p->engine, span_begin(&p->scanner), MM_ERR_UNEXPECTED_EOF);

  if (ch == '_' || isalpha(ch))
    return tok_ident(p);

  if (match_str(&p->scanner, "$header")) {
    MMToken tok = {0};
    tok.type = MMT_HEADERPAIR;

    skip_unwanted(&p->scanner);
    expect_mmtok(p, MMT_EQ);
    skip_unwanted(&p->scanner);

    Span start_span = span_begin(&p->scanner);
    int next_c = peekch(&p->scanner);
    if (next_c == '"')
      tok.sv = tok_str(p).sv;
    else if (next_c == '<')
      tok.sv = tok_angstr(p).sv;
    else
      throw_diag(&p->engine, tok.span, MM_ERR_HEADER_FILE_NOT_IN_DOUBLE_QUOTES);

    span_end(&start_span, &p->scanner);

    tok.span = start_span;
    tok.sv = span_sv(p->sman, start_span);
    return tok;
  }

  switch (ch) {
  case '=':
    return tok_simple(p, MMT_EQ);
  case '}':
    return tok_simple(p, MMT_RCURLY);
  case '{':
    return tok_simple(p, MMT_LCURLY);
  case '"':
    return tok_str(p);
  case '<':
    return tok_angstr(p);
  case '$':
    return tok_keyw(p);
  }

  throw_diag(&p->engine, span_begin(&p->scanner), MM_ERR_UNEXPECTED_CHAR, ch);
}

const bstr mmtok_type_to_str[] = {
    "=", "{", "}", "STR", "ANGSTR", "KEYW", "IDENT", "HEADERPAIR", "PAIR", "AS"};

MMToken expect_mmtok(MMParser* p, MMTokenType type) {
  MMToken tok = get_mmtok(p);
  if (tok.type != type) {
    throw_diag(
        &p->engine,
        tok.span,
        MM_ERR_UNEXPECTED_TOK,
        mmtok_type_to_str[type],
        mmtok_type_to_str[tok.type]);
  }
  return tok;
}
