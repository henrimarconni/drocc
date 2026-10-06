#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chucci_preproc/define.h"
#include "chucci_preproc/preproc.h"
#include "core/bitarr.h"
#include "core/infvec.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <stdint.h>
#include <stdlib.h>

Token ms_next(void* ms) {
  MacroStream* m = ms;
  if (m->len == 0)
    return EOF_TOKEN;

  m->len--;
  return *(m->start++);
}

Token ms_peek(void* ms) {
  MacroStream* m = ms;
  if (m->len == 0)
    return EOF_TOKEN;

  return *(m->start++);
}

void ms_reset(void* ms) {
  MacroStream* m = ms;
  m->start -= m->og_len - m->len;
  m->len = m->og_len;
}

void ms_free(void** ms) { (void)ms; }

void define_macro(Preprocessor* pp) {
  Token name = tstack_next(&pp->stack);
  Token next = tstack_next(&pp->stack);

  // Function like
  if (name.span.offset + name.span.len == next.span.offset && next.kind == SEP_LPAREN) {
  }

  // Object like
  Token* start = pp->tokencache.get + pp->tokencache.n;

  while (next.kind != SEP_NEWLINE && next.kind != TOK_EOF) {
    infvec_push(pp->tokencache, next);
    next = tstack_next(&pp->stack);
  }

  MacroStream* ms = vmarena_alloc(pp->arena, sizeof(MacroStream));
  ms->len = (uint32_t)(pp->tokencache.get - start);
  ms->og_len = ms->len;
  ms->start = start;
  TokenStream ts = ts_from_func(ms, ms_next, ms_peek, ms_free, ms_reset);
  vec_push(pp->stack, ts);
}

// TODO
MacroManager* macroman_new() {
  MacroManager* man = malloc(sizeof(MacroManager));
  return man;
}
void macroman_free(MacroManager* man);
void macroman_add(MacroManager* man, InternID name, TokenStream ts);
void macroman_remove(MacroManager* man, InternID name);
