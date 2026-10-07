#include "chu80/diag.h"
#include "chu80/lexer.h"
#include "chu80/parser.h"
#include "chu80/strmap.h"
#include "core/diagnostics.h"
#include "core/srcman.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

C80Parser c80_new(C80Lexer l) {
  C80Parser p = {0};
  p.arena = vmarena_new(1024 * 1024);
  p.map = strmap_new(p.arena);
  p.l = l;
  p.pc = 0;
  return p;
}

void c80_pass1(C80Parser* p) {
  C80Token tok = c80_lex(&p->l);

  while (tok.kind != C80_EOF) {
    if (tok.kind == C80_LABEL) {
      SMSpanInfo info = sman_info(p->l.sman, tok.span);
      strmap_put(p->map, info.sv, p->pc);
    }

    int bytes = c80_token_instruction_size(tok);
    p->pc += bytes;

    tok = c80_lex(&p->l);
  }

  p->pc = 0;
  c80lex_reset(&p->l);
}

// ----------------------------- HELPER UTILITIES -----------------------------

static uint8_t reg_code(C80Parser* p, C80Token reg) {
  if (reg.kind <= C80REG_START || reg.kind >= C80REG_END)
    throw_diag(&p->l.engine, reg.span, C80_ERR_INVALID_REG);
  return reg.kind - C80REG_B;
}

static uint8_t rp_code_arith(C80Parser* p, C80Token reg) {
  if (reg.kind == C80REG_B)
    return 0;
  if (reg.kind == C80REG_D)
    return 1;
  if (reg.kind == C80REG_H)
    return 2;
  if (reg.kind == C80REG_SP)
    return 3;
  throw_diag(&p->l.engine, reg.span, C80_ERR_INVALID_REG);
}

static uint8_t rp_code_stack(C80Parser* p, C80Token reg) {
  if (reg.kind == C80REG_B)
    return 0;
  if (reg.kind == C80REG_D)
    return 1;
  if (reg.kind == C80REG_H)
    return 2;
  if (reg.kind == C80_PSW)
    return 3;
  throw_diag(&p->l.engine, reg.span, C80_ERR_INVALID_REG);
}

static void emit_u8(C80Parser* p, uint8_t opcode) {
  vec_push(p->emitted, opcode);
  C80Token data = c80_lex(&p->l);
  if (data.num > 255)
    throw_diag(&p->l.engine, data.span, C80_ERR_LARGER_THAN_1_BYTE, data.num);
  vec_push(p->emitted, (uint8_t)data.num);
}

static uint16_t resolve_val16(C80Parser* p, C80Token tok) {
  if (tok.kind == C80_INT)
    return tok.num;
  if (tok.kind == C80_IDENT) {
    SMSpanInfo info = sman_info(p->l.sman, tok.span);
    int addr = 0;
    bool res = strmap_get(p->map, info.sv, &addr);
    if (!res)
      throw_diag(&p->l.engine, tok.span, C80_ERR_UNDEFINED_SYMBOL);
    return addr;
  }
  throw_diag(&p->l.engine, tok.span, C80_ERR_UNEXPECTED_EXPR);
  return 0;
}

static void emit_u16(C80Parser* p, uint8_t opcode) {
  vec_push(p->emitted, opcode);
  C80Token tok = c80_lex(&p->l);
  uint16_t val = resolve_val16(p, tok);
  vec_push(p->emitted, (uint8_t)(val & 0xFF));
  vec_push(p->emitted, (uint8_t)((val >> 8) & 0xFF));
}

// ----------------------------- ONE BYTE INSTRUCTIONS ------------------------------

static void emit_nop(C80Parser* p) { vec_push(p->emitted, 0x00); }
static void emit_hlt(C80Parser* p) { vec_push(p->emitted, 0x76); }
static void emit_ret(C80Parser* p) { vec_push(p->emitted, 0xC9); }
static void emit_pchl(C80Parser* p) { vec_push(p->emitted, 0xE9); }
static void emit_sphl(C80Parser* p) { vec_push(p->emitted, 0xF9); }
static void emit_xchg(C80Parser* p) { vec_push(p->emitted, 0xEB); }
static void emit_xthl(C80Parser* p) { vec_push(p->emitted, 0xE3); }
static void emit_rlc(C80Parser* p) { vec_push(p->emitted, 0x07); }
static void emit_rrc(C80Parser* p) { vec_push(p->emitted, 0x0F); }
static void emit_ral(C80Parser* p) { vec_push(p->emitted, 0x17); }
static void emit_rar(C80Parser* p) { vec_push(p->emitted, 0x1F); }

static void emit_mov(C80Parser* p) {
  C80Token tok_reg1 = c80_lex(&p->l);
  uint8_t reg1 = reg_code(p, tok_reg1);
  uint8_t reg2 = reg_code(p, c80_lex(&p->l));

  if (reg1 == reg2 && reg1 == 0b110)
    throw_diag(&p->l.engine, tok_reg1.span, C80_ERR_INVALID_MOV);

  vec_push(p->emitted, 0b01000000 | (reg1 << 3) | reg2);
}

static void emit_stax_ldax(C80Parser* p, bool is_ldax) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b00000010 | (is_ldax << 3);

  if (reg.kind == C80REG_B)
    ;
  else if (reg.kind == C80REG_D)
    code |= (1 << 4);
  else
    throw_diag(&p->l.engine, reg.span, C80_ERR_INVALID_REG);

  vec_push(p->emitted, code);
}

static void emit_stax(C80Parser* p) { emit_stax_ldax(p, false); }
static void emit_ldax(C80Parser* p) { emit_stax_ldax(p, true); }

static void emit_alu_reg(C80Parser* p, uint8_t base_op) {
  vec_push(p->emitted, base_op | reg_code(p, c80_lex(&p->l)));
}

static void emit_add(C80Parser* p) { emit_alu_reg(p, 0b10000000); }
static void emit_adc(C80Parser* p) { emit_alu_reg(p, 0b10001000); }
static void emit_sub(C80Parser* p) { emit_alu_reg(p, 0b10010000); }
static void emit_sbb(C80Parser* p) { emit_alu_reg(p, 0b10011000); }
static void emit_ana(C80Parser* p) { emit_alu_reg(p, 0b10100000); }
static void emit_xra(C80Parser* p) { emit_alu_reg(p, 0b10101000); }
static void emit_ora(C80Parser* p) { emit_alu_reg(p, 0b10110000); }
static void emit_cmp(C80Parser* p) { emit_alu_reg(p, 0b10111000); }

static void emit_push(C80Parser* p) {
  vec_push(p->emitted, 0b11000101 | (rp_code_stack(p, c80_lex(&p->l)) << 4));
}

static void emit_pop(C80Parser* p) {
  vec_push(p->emitted, 0b11000001 | (rp_code_stack(p, c80_lex(&p->l)) << 4));
}

static void emit_dad(C80Parser* p) {
  vec_push(p->emitted, 0b00001001 | (rp_code_arith(p, c80_lex(&p->l)) << 4));
}

static void emit_inx(C80Parser* p) {
  vec_push(p->emitted, 0b00000011 | (rp_code_arith(p, c80_lex(&p->l)) << 4));
}

static void emit_dcx(C80Parser* p) {
  vec_push(p->emitted, 0b00001011 | (rp_code_arith(p, c80_lex(&p->l)) << 4));
}

// ----------------------------- TWO BYTE INSTRUCTIONS ------------------------------

static void emit_mvi(C80Parser* p) {
  uint8_t reg = reg_code(p, c80_lex(&p->l));
  emit_u8(p, 0b00000110 | (reg << 3));
}

static void emit_adi(C80Parser* p) { emit_u8(p, 0b11000110); }
static void emit_aci(C80Parser* p) { emit_u8(p, 0b11001110); }
static void emit_sui(C80Parser* p) { emit_u8(p, 0b11010110); }
static void emit_sbi(C80Parser* p) { emit_u8(p, 0b11011110); }
static void emit_ani(C80Parser* p) { emit_u8(p, 0b11100110); }
static void emit_xri(C80Parser* p) { emit_u8(p, 0b11101110); }
static void emit_ori(C80Parser* p) { emit_u8(p, 0b11110110); }
static void emit_in(C80Parser* p) { emit_u8(p, 0b11011011); }
static void emit_out(C80Parser* p) { emit_u8(p, 0b11010011); }

// ---------------------------- THREE BYTE INSTRUCTIONS -----------------------------

static void emit_jmp(C80Parser* p) { emit_u16(p, 0b11000011); }
static void emit_jc(C80Parser* p) { emit_u16(p, 0b11011010); }
static void emit_jnc(C80Parser* p) { emit_u16(p, 0b11010010); }
static void emit_jz(C80Parser* p) { emit_u16(p, 0b11001010); }
static void emit_jnz(C80Parser* p) { emit_u16(p, 0b11000010); }
static void emit_jm(C80Parser* p) { emit_u16(p, 0b11111010); }
static void emit_jp(C80Parser* p) { emit_u16(p, 0b11110010); }
static void emit_jpe(C80Parser* p) { emit_u16(p, 0b11101010); }
static void emit_jpo(C80Parser* p) { emit_u16(p, 0b11100010); }
static void emit_call(C80Parser* p) { emit_u16(p, 0b11001101); }
static void emit_sta(C80Parser* p) { emit_u16(p, 0b00110010); }
static void emit_lda(C80Parser* p) { emit_u16(p, 0b00111010); }
static void emit_shld(C80Parser* p) { emit_u16(p, 0b00100010); }
static void emit_lhld(C80Parser* p) { emit_u16(p, 0b00101010); }
static void emit_lxi(C80Parser* p) {
  uint8_t rp = rp_code_arith(p, c80_lex(&p->l));
  emit_u16(p, 0b00000001 | (rp << 4));
}

// -------------------------------- PASS 2 DISPATCH ---------------------------------

static void emit_org(C80Parser* p, C80Token tok) {
  if (p->pc > tok.org_num)
    throw_diag(&p->l.engine, tok.span, C80_ERR_INVALID_INT);

  int i = tok.org_num - p->pc;
  while (i--)
    vec_push(p->emitted, 0xFF);
}

static void emit_op(C80Parser* p, C80Token op) {
  if (op.kind == PSEUDO_ORG) {
    emit_org(p, op);
    return;
  }
  switch (op.kind) {
#define X(kind, name, size, func)                                                                  \
  case kind:                                                                                       \
    func(p);                                                                                       \
    return;
    TOKENS(X)
#undef X
  default:
    assert(false && "Unimplemented");
  }
}

void c80_pass2(C80Parser* p) {
  C80Token tok = c80_lex(&p->l);

  while (tok.kind != C80_EOF) {
    if (!c80_is_opcode(tok.kind)) {
      tok = c80_lex(&p->l);
      continue;
    }
    emit_op(p, tok);
    p->pc += c80_token_instruction_size(tok);
    tok = c80_lex(&p->l);
  }

  for (uint i = 0; i < p->emitted.n; i++) {
    printf(" %x ", p->emitted.get[i]);
  }
  puts("");
}
