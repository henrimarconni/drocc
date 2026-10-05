#ifndef IR_H_
#define IR_H_

#include "chucci_parse/parser.h"
#include "core/srcman.h"
#include "core/vmem_arena.h"

typedef struct {
  Parser* p;
  VMEMArena* arena;
  SourceManager* sman;
} IRStream;

IRStream irstream_new(Parser* p, VMEMArena* arena);

#endif
