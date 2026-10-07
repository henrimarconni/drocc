#ifndef LEXER_H_
#define LEXER_H_

#include "core/diagnostics.h"
#include "core/srcman.h"
#include <setjmp.h>

#define TOKENS(X)                                                                                  \
  /* 1-byte */                                                                                     \
  X(OP_NOP, "nop", 1, emit_nop)                                                                    \
  X(OP_MOV, "mov", 1, emit_mov)                                                                    \
  X(OP_ADD, "add", 1, emit_add)                                                                    \
  X(OP_ADC, "adc", 1, emit_adc)                                                                    \
  X(OP_ANA, "ana", 1, emit_ana)                                                                    \
  X(OP_XRA, "xra", 1, emit_xra)                                                                    \
  X(OP_ORA, "ora", 1, emit_ora)                                                                    \
  X(OP_SUB, "sub", 1, emit_sub)                                                                    \
  X(OP_SBB, "sbb", 1, emit_sbb)                                                                    \
  X(OP_LDAX, "ldax", 1, emit_ldax)                                                                 \
  X(OP_STAX, "stax", 1, emit_stax)                                                                 \
  X(OP_HLT, "hlt", 1, emit_hlt)                                                                    \
  X(OP_CMP, "cmp", 1, emit_cmp)                                                                    \
  X(OP_RLC, "rlc", 1, emit_rlc)                                                                    \
  X(OP_RRC, "rrc", 1, emit_rrc)                                                                    \
  X(OP_RAL, "ral", 1, emit_ral)                                                                    \
  X(OP_RAR, "rar", 1, emit_rar)                                                                    \
  X(OP_RET, "ret", 1, emit_ret)                                                                    \
  X(OP_PUSH, "push", 1, emit_push)                                                                 \
  X(OP_POP, "pop", 1, emit_pop)                                                                    \
  X(OP_DAD, "dad", 1, emit_dad)                                                                    \
  X(OP_INX, "inx", 1, emit_inx)                                                                    \
  X(OP_DCX, "dcx", 1, emit_dcx)                                                                    \
  X(OP_XCHG, "xchg", 1, emit_xchg)                                                                 \
  X(OP_XTHL, "xthl", 1, emit_xthl)                                                                 \
  X(OP_SPHL, "sphl", 1, emit_sphl)                                                                 \
  X(OP_PCHL, "pchl", 1, emit_pchl)                                                                 \
  /* 2-byte */                                                                                     \
  X(OP_MVI, "mvi", 2, emit_mvi)                                                                    \
  X(OP_ACI, "aci", 2, emit_aci)                                                                    \
  X(OP_ADI, "adi", 2, emit_adi)                                                                    \
  X(OP_SUI, "sui", 2, emit_sui)                                                                    \
  X(OP_SBI, "sbi", 2, emit_sbi)                                                                    \
  X(OP_ANI, "ani", 2, emit_ani)                                                                    \
  X(OP_XRI, "xri", 2, emit_xri)                                                                    \
  X(OP_ORI, "ori", 2, emit_ori)                                                                    \
  X(OP_IN, "in", 2, emit_in)                                                                       \
  X(OP_OUT, "out", 2, emit_out)                                                                    \
  /* 3-byte */                                                                                     \
  X(OP_JMP, "jmp", 3, emit_jmp)                                                                    \
  X(OP_JC, "jc", 3, emit_jc)                                                                       \
  X(OP_JNC, "jnc", 3, emit_jnc)                                                                    \
  X(OP_JZ, "jz", 3, emit_jz)                                                                       \
  X(OP_JNZ, "jnz", 3, emit_jnz)                                                                    \
  X(OP_JM, "jm", 3, emit_jm)                                                                       \
  X(OP_JP, "jp", 3, emit_jp)                                                                       \
  X(OP_JPE, "jpe", 3, emit_jpe)                                                                    \
  X(OP_JPO, "jpo", 3, emit_jpo)                                                                    \
  X(OP_CALL, "call", 3, emit_call)                                                                 \
  X(OP_SHLD, "shld", 3, emit_shld)                                                                 \
  X(OP_LHLD, "lhld", 3, emit_lhld)                                                                 \
  X(OP_STA, "sta", 3, emit_sta)                                                                    \
  X(OP_LXI, "lxi", 3, emit_lxi)                                                                    \
  X(OP_LDA, "lda", 3, emit_lda)

// X(OP_CPI, "cpi", 2, emit_cpi)

typedef enum {
  C80_IDENT,
  C80_LABEL,
  C80_INT,
  C80_PC,
  C80_EOF,
  PSEUDO_ORG,

  C80REG_START,
  C80REG_B,
  C80REG_C,
  C80REG_D,
  C80REG_E,
  C80REG_H,
  C80REG_L,
  C80REG_M,
  C80REG_A,
  C80REG_SP,
  C80REG_END,

  C80_PSW,

  C80_OPCODE_START,
#define X(a, b, c, d) a,
  TOKENS(X)
#undef X
      C80_OPCODE_END
} C80TokenKind;

typedef struct {
  C80TokenKind kind;
  Span span;
  union {
    uint16_t num;
    uint16_t org_num;
  };
} C80Token;

typedef struct {
  SrcScanner prev_scanner;
  SrcScanner scanner;
  SourceManager* sman;
  DiagEngine engine;
} C80Lexer;

C80Lexer c80lex_new(SrcScanner scanner, SourceManager* sman, jmp_buf* onerror);
int c80_token_instruction_size(C80Token tok);
static inline int c80_is_opcode(C80TokenKind kind) {
  return (kind > C80_OPCODE_START && kind < C80_OPCODE_END) || PSEUDO_ORG == kind;
}
void c80lex_reset(C80Lexer* l);
C80Token c80_lex(C80Lexer* l);

#endif
