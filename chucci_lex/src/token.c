#include "chucci_lex/token.h"
#include "core/span.h"
#include "core/string_interner.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

const char* tok_to_str[__token_kind_count] = {
#define X(a, b) b,
    KEYWORDS(X)
#undef X
#define X(a, b, c) b,
        OPERATORS(X)
#undef X
#define X(a, b, c) b,
            SEPARATORS(X)
#undef X
                "eof",
    "ident", "value"};
#define X(a, b, c) [(unsigned char) c] = true,

// Ignore the warning, it is due to the fact that multiple operators start with same character
const bool is_op_table[256] = {OPERATORS(X)};
const bool is_sep_table[256] = {SEPARATORS(X)};
#undef X

Token new_tok_ident(Span span, InternID name) {
  Token token = {0};
  token.kind = TOK_IDENT;
  token.ident = name;
  token.span = span;
  return token;
}

Token new_tok_val(Span span) {
  Token token = {0};
  token.kind = TOK_VAL;
  token.span = span;
  return token;
}

Token new_tok_simple(Span span, TokenKind kind) {
  Token token;
  token.kind = kind;
  token.span = span;
  return token;
}

void print_token(Token* token) {
  printf("%s: %.*s", tok_to_str[token->kind], span_fmt(token->span));
}
void print_token_pretty(Token* token) {
  if (token->kind == SEP_NEWLINE) {
    printf("sep(\\n)");
    return;
  }
  switch (token->kind) {
#define X(a, b, c)                                                                                 \
  case a:                                                                                          \
    printf("op(%s)", b);                                                                           \
    \ break;                                                                                       \
    OPERATORS(X)
#undef X
#define X(a, b, c)                                                                                 \
  case a:                                                                                          \
    printf("sep(%s)", b);                                                                          \
    \ break;                                                                                       \
    SEPARATORS(X)
#undef X
#define X(a, b)                                                                                    \
  case a:                                                                                          \
    printf("keyword(%s)", b);                                                                      \
    \ break;                                                                                       \
    KEYWORDS(X)
#undef X
  case TOK_EOF:
    printf("eof");
    break;
  case TOK_VAL:
    printf("val(%.*s)", span_fmt(token->span));
    break;
  case TOK_IDENT:
    printf("ident(%.*s)", span_fmt(token->span));
    break;
  default:
    assert(false && "UNREACHABLE");
  }
}
