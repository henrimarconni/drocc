#ifndef PARSER_H_
#define PARSER_H_

#include "chu80/strmap.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "lexer.h"
#include <stdint.h>

typedef struct {
  VMEMArena* arena;
  StrToIntMap* map;
  C80Lexer l;
  vec(uint8_t) emitted;
  int pc;
} C80Parser;

C80Parser c80_new(C80Lexer l);
void c80_pass1(C80Parser* p);
void c80_pass2(C80Parser* p);

#endif
