#ifndef AMALGAMATE_H
#define AMALGAMATE_H

#include "core/stringbuilder.h"
#include "core/vmem_arena.h"
#include <setjmp.h>

typedef vec(bstr) IncludeDirVec;
typedef vec(bstr) InputFIleVec;

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, InputFIleVec input_files, jmp_buf* onerror);

#endif
