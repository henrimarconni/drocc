#include "chu80/diag.h"
#include "chu80/lexer.h"
#include "chu80/parser.h"
#include "chu80/strmap.h"
#include "core/diagnostics.h"
#include "core/srcman.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
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

    int bytes = c80_token_instruction_size(tok.kind);
    p->pc += bytes;

    tok = c80_lex(&p->l);
  }

  c80lex_reset(&p->l);
}

static void emit_nop(C80Parser* p) { vec_push(p->emitted, 0); }

static uint8_t reg_code(C80Parser* p, C80Token reg) {
  if (reg.kind <= C80REG_START || reg.kind >= C80REG_END)
    throw_diag(&p->l.engine, reg.span, C80_ERR_INVALID_REG);
  return reg.kind - C80REG_B;
}

static void emit_mov(C80Parser* p) {
  C80Token tok_reg1 = c80_lex(&p->l);
  uint8_t reg1 = reg_code(p, tok_reg1);
  uint8_t reg2 = reg_code(p, c80_lex(&p->l));

  if (reg1 == reg2 && reg1 == 0b110)
    throw_diag(&p->l.engine, tok_reg1.span, C80_ERR_INVALID_MOV);

  reg1 <<= 3;

  uint8_t code = 0b01000000;
  code |= reg1;
  code |= reg2;

  vec_push(p->emitted, code);
}

static void emit_stax_ldax(C80Parser* p, bool stax) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b00000010;
  code |= stax << 3;

  if (reg.kind == C80REG_B)
    ;
  else if (reg.kind == C80REG_D)
    code |= 1 << 4;
  else
    throw_diag(&p->l.engine, reg.span, C80_ERR_INVALID_REG);

  vec_push(p->emitted, code);
}

static void emit_add(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10000000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_sub(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10010000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_sbb(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10011000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_adc(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10001000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_ana(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10100000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_xra(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10101000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_ora(C80Parser* p) {
  C80Token reg = c80_lex(&p->l);
  uint8_t code = 0b10110000;
  code |= reg_code(p, reg);
  vec_push(p->emitted, code);
}

static void emit_stax(C80Parser* p) { emit_stax_ldax(p, false); }

static void emit_ldax(C80Parser* p) { emit_stax_ldax(p, true); }

static void emit_hlt(C80Parser* p) { vec_push(p->emitted, 0b01110110); }

static void emit_op(C80Parser* p, C80Token op) {
  switch (op.kind) {
  case OP_NOP:
    emit_nop(p);
    return;
  case OP_MOV:
    emit_mov(p);
    return;
  case OP_LDAX:
    emit_ldax(p);
    return;
  case OP_STAX:
    emit_stax(p);
    return;
  case OP_HLT:
    emit_hlt(p);
    return;
  case OP_ADD:
    emit_add(p);
    return;
  case OP_ADC:
    emit_adc(p);
    return;
  case OP_SUB:
    emit_sub(p);
    return;
  case OP_SBB:
    emit_sbb(p);
    return;
  case OP_ANA:
    emit_ana(p);
    return;
  case OP_XRA:
    emit_xra(p);
    return;
  case OP_ORA:
    emit_ora(p);
    return;
  default:
    assert(false && "Unimplemented");
  }
}

void c80_pass2(C80Parser* p) {
  C80Token tok = c80_lex(&p->l);

  while (tok.kind != C80_EOF) {
    if (!c80_is_opcode(tok.kind))
      tok = c80_lex(&p->l);
    emit_op(p, tok);
    tok = c80_lex(&p->l);
  }

  for (uint32_t i = 0; i < p->emitted.n; i++)
    printf("%x ", p->emitted.get[i]);
  puts("");
}
