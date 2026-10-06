#ifndef LEXER_H_
#define LEXER_H_

#include "core/diagnostics.h"
#include "core/srcman.h"
#include <setjmp.h>

#define TOKENS(X)                                                                                  \
  /* 1-byte */                                                                                     \
  X(OP_NOP, "nop", 1)                                                                              \
  X(OP_MOV, "mov", 1)                                                                              \
  X(OP_ADD, "add", 1)                                                                              \
  X(OP_ADC, "adc", 1)                                                                              \
  X(OP_ANA, "ana", 1)                                                                              \
  X(OP_XRA, "xra", 1)                                                                              \
  X(OP_ORA, "ora", 1)                                                                              \
  X(OP_SUB, "sub", 1)                                                                              \
  X(OP_SBB, "sbb", 1)                                                                              \
  X(OP_LDAX, "ldax", 1)                                                                            \
  X(OP_STAX, "stax", 1)                                                                            \
  X(OP_HLT, "hlt", 1)                                                                              \
  X(OP_CMP, "cmp", 1)                                                                              \
  X(OP_RLC, "rlc", 1)                                                                              \
  X(OP_RRC, "rrc", 1)                                                                              \
  X(OP_RAL, "ral", 1)                                                                              \
  X(OP_RAR, "rar", 1)                                                                              \
  X(OP_RET, "ret", 1)                                                                              \
  X(OP_PUSH, "push", 1)                                                                            \
  X(OP_POP, "pop", 1)                                                                              \
  /* 2-byte */                                                                                     \
  X(OP_MVI, "mvi", 2)                                                                              \
  X(OP_ADI, "adi", 2)                                                                              \
  X(OP_SUI, "sui", 2)                                                                              \
  X(OP_IN, "in", 2)                                                                                \
  X(OP_OUT, "out", 2)                                                                              \
  /* 3-byte */                                                                                     \
  X(OP_LXI, "lxi", 3)                                                                              \
  X(OP_JMP, "jmp", 3)                                                                              \
  X(OP_CALL, "call", 3)                                                                            \
  X(OP_STA, "sta", 3)                                                                              \
  X(OP_LDA, "lda", 3)

typedef enum {
  C80_IDENT,
  C80_LABEL,
  C80_INT,
  C80_PC,
  C80_EOF,

  C80REG_START,
  C80REG_B,
  C80REG_C,
  C80REG_D,
  C80REG_E,
  C80REG_H,
  C80REG_L,
  C80REG_M,
  C80REG_A,
  C80REG_END,

  C80_PSW,

  C80_OPCODE_START,
#define X(a, b, c) a,
  TOKENS(X)
#undef X
      C80_OPCODE_END
} C80TokenKind;

typedef struct {
  C80TokenKind kind;
  Span span;
} C80Token;

typedef struct {
  SrcScanner prev_scanner;
  SrcScanner scanner;
  SourceManager* sman;
  DiagEngine engine;
} C80Lexer;

C80Lexer c80lex_new(SrcScanner scanner, SourceManager* sman, jmp_buf* onerror);
int c80_token_instruction_size(C80TokenKind kind);
static inline int c80_is_opcode(C80TokenKind kind) {
  return kind > C80_OPCODE_START && kind < C80_OPCODE_END;
}
void c80lex_reset(C80Lexer* l);
C80Token c80_lex(C80Lexer* l);

#endif
