#ifndef AMALGAMATE_H
#define AMALGAMATE_H

#include "core/stringbuilder.h"
#include "core/vmem_arena.h"
#include <setjmp.h>
#include "core/srcman.h"
#include "core/diagnostics.h"

typedef vec(bstr) IncludeDirVec;
typedef vec(bstr) InputFIleVec;

typedef struct {
  StringBuilder output;
  IncludeDirVec idirs;
  InputFIleVec files;
  SourceManager* sman;
  VMEMArena* arena;
  DiagEngine engine;
} Amalgamator;

StringBuilder amalgamate(Amalgamator* a, jmp_buf* onerror);
void amalgamator_free(Amalgamator* a);

#endif
