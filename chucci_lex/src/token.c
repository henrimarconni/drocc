#include "chucci_lex/token.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/stringdef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

const char* tok_to_str[__token_kind_count] = {
#define X(a, b) b,
    KEYWORDS(X)
#undef X
        "__keyword_count",
#define X(a, b, c) b,
    OPERATORS(X)
#undef X
#define X(a, b, c) b,
        SEPARATORS(X)
#undef X
    // NOTE: Update this if you add more token types
    "eof",
    "ident",
    "string",
    "value",
    "anglestr"};
#define X(a, b, c) [(unsigned char) c] = true,

// Ignore the warning, it is due to the fact that multiple operators start with same character
const bool is_op_table[256] = {OPERATORS(X)};
const bool is_sep_table[256] = {SEPARATORS(X)};
#undef X

void print_token(SourceManager* sman, Token* token) {
  if (token->span.srcid == INVALID_SRC_ID) {
    printf("eof");
    return;
  }
  StringView sv = span_sv(sman, token->span);
  printf("%s(%.*s)", tok_to_str[token->kind], sv.len, sv.str);
}

Token token_new(Span span, TokenKind kind) {
  Token token = {0};
  token.span = span;
  token.kind = kind;
  return token;
}

Token token_new_ident(Span span, TokenKind kind, InternID ident) {
  Token token = {0};
  token.span = span;
  token.kind = kind;
  token.ident = ident;
  return token;
}
