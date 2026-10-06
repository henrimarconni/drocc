#include "chu80/diag.h"
#include "chu80/lexer.h"
#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const unsigned char instruction_sizes[] = {
    0, // C80_OPCODE_START
#define X(a, b, size) size,
    TOKENS(X)
#undef X
};

int c80_token_instruction_size(C80TokenKind kind) {
  if (c80_is_opcode(kind)) {
    return instruction_sizes[kind - C80_OPCODE_START];
  }
  return 0; // Not an instruction
}

static C80TokenKind lookup_keyword(StringView sv) {
#define X(a, name, size)                                                                           \
  if (strlen(name) == sv.len && strncasecmp(sv.str, name, sv.len) == 0)                            \
    return a;
  TOKENS(X)
#undef X
  return C80_IDENT;
}

C80Lexer c80lex_new(SrcScanner scanner, SourceManager* sman, jmp_buf* onerror) {
  C80Lexer l = {0};
  l.scanner = scanner;
  l.sman = sman;
  l.engine = new_engine(c80_diaginfos, __c80_diaginfos_len, sman, onerror);
  l.prev_scanner = scanner;
  return l;
}

C80Token c80_lex(C80Lexer* l) {
  skip_space(&l->scanner);
  C80Token tok = {0};

  Span span = span_begin(&l->scanner);
  char ch = nextch(&l->scanner);

  if (isalpha(ch) || ch == '?' || ch == '!') {
    ch = peekch(&l->scanner);
    while (!isspace(ch) && ch != EOF && ch != ',' && ch != ':') {
      nextch(&l->scanner);
      ch = peekch(&l->scanner);
    }

    span_end(&span, &l->scanner);
    tok.span = span;
    SMSpanInfo info = sman_info(l->sman, span);
    C80TokenKind kind = lookup_keyword(info.sv);

    if (ch == ':') {
      nextch(&l->scanner);
      ch = peekch(&l->scanner);
      tok.kind = C80_LABEL;
      if (c80_is_opcode(kind))
        throw_diag(&l->engine, span, C80_ERR_INVALID_LABEL, info.sv);
    }

    if (span.len == 1) {
      switch (info.sv.str[0]) {
      case 'A':
        kind = C80REG_A;
        break;
      case 'B':
        kind = C80REG_B;
        break;
      case 'C':
        kind = C80REG_C;
        break;
      case 'D':
        kind = C80REG_D;
        break;
      case 'E':
        kind = C80REG_E;
        break;
      case 'H':
        kind = C80REG_H;
        break;
      case 'L':
        kind = C80REG_L;
        break;
      case 'M':
        kind = C80REG_M;
        break;
      }
    }

    tok.kind = kind;
    return tok;
  }

  // Number
  if (isdigit(ch)) {
    while (isalnum(ch)) {
      nextch(&l->scanner);
      ch = peekch(&l->scanner);
    }
    span_end(&span, &l->scanner);
    tok.kind = C80_INT;
    tok.span = span;
    return tok;
  }

  // Program counter
  if (ch == '$') {
    span_end(&span, &l->scanner);
    tok.kind = C80_PC;
    tok.span = span;
    return tok;
  }

  if (ch == ';') {
    while (ch != '\n' && ch != EOF)
      ch = nextch(&l->scanner);
    return c80_lex(l); // consume comment and get next token
  }

  if (ch == EOF) {
    span_end(&span, &l->scanner);
    tok.kind = C80_EOF;
    tok.span = span;
    return tok;
  }

  span_end(&span, &l->scanner);
  throw_diag(&l->engine, span, C80_ERR_INVALID_CHAR, ch);
}

void c80lex_reset(C80Lexer* l) { l->scanner = l->prev_scanner; }
