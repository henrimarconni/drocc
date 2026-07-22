#ifndef AMALGAMATE_H
#define AMALGAMATE_H

#include "stringbuilder.h"
#include "vmem_arena.h"
#include <setjmp.h>

typedef vec(bstr) IncludeDirVec;

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, bstr output, bstr input, jmp_buf* onerror);

#endif
