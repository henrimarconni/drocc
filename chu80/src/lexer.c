#include "chu80/diag.h"
#include "chu80/lexer.h"
#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

static const unsigned char instruction_sizes[] = {
    0, // C80_OPCODE_START
#define X(a, b, size, d) size,
    TOKENS(X)
#undef X
};

int c80_token_instruction_size(C80Token tok) {
  if (tok.kind == PSEUDO_ORG)
    return tok.org_num;
  if (c80_is_opcode(tok.kind)) {
    return instruction_sizes[tok.kind - C80_OPCODE_START];
  }
  return 0; // Not an instruction
}

static C80TokenKind lookup_keyword(StringView sv) {
#define X(a, name, size, d)                                                                        \
  if (strlen(name) == sv.len && strncasecmp(sv.str, name, sv.len) == 0)                            \
    return a;
  TOKENS(X)
#undef X
  if (3 == sv.len && strncasecmp("psw", sv.str, 3) == 0)
    return C80_PSW;
  if (3 == sv.len && strncasecmp("sp", sv.str, 3) == 0)
    return C80_PSW;
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
  char ch = peekch(&l->scanner);

  if (isalpha(ch) || ch == '?' || ch == '!') {
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
      kind = C80_LABEL;
      if (c80_is_opcode(kind))
        throw_diag(&l->engine, span, C80_ERR_INVALID_LABEL, info.sv);
    }

    if (span.len == 1) {
      switch (info.sv.str[0]) {
      case 'a':
      case 'A':
        kind = C80REG_A;
        break;
      case 'b':
      case 'B':
        kind = C80REG_B;
        break;
      case 'c':
      case 'C':
        kind = C80REG_C;
        break;
      case 'd':
      case 'D':
        kind = C80REG_D;
        break;
      case 'e':
      case 'E':
        kind = C80REG_E;
        break;
      case 'h':
      case 'H':
        kind = C80REG_H;
        break;
      case 'l':
      case 'L':
        kind = C80REG_L;
        break;
      case 'm':
      case 'M':
        kind = C80REG_M;
        break;
      }
    }

    if (info.sv.len == 3 && strncasecmp("org", info.sv.str, 3) == 0) {
      C80Token num = c80_lex(l);
      if (num.kind != C80_INT)
        throw_diag(&l->engine, num.span, C80_ERR_INVALID_INT);
      kind = PSEUDO_ORG;
      tok.org_num = num.num;
    }

    tok.kind = kind;
    return tok;
  }

  // Number
  if (isdigit(ch)) {
    char buf[32];
    int len = 0;

    while ((isalnum(ch)) && len < 31) {
      buf[len++] = ch;
      ch = nextch(&l->scanner);
      ch = peekch(&l->scanner);
    }
    buf[len] = '\0';

    uint16_t num = 0;
    char* endptr;

    if (len > 0 && (buf[len - 1] == 'h' || buf[len - 1] == 'H')) {
      buf[len - 1] = '\0';
      num = (uint16_t)strtol(buf, &endptr, 16);
    } else {
      num = (uint16_t)strtol(buf, &endptr, 10);
    }

    span_end(&span, &l->scanner);
    tok.kind = C80_INT;
    tok.span = span;
    tok.num = num;
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
