#include "amal_diag.h"
#include "amalgamate.h"
#include "diagnostics.h"
#include "scanner.h"
#include "span.h"
#include "stringbuilder.h"
#include "vmem_arena.h"
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  StringBuilder output;
  IncludeDirVec include_dirs;
  SourceFile file;
  VMEMArena* arena;
  DiagEngine engine;
} Amalgamator;

void try_find(SourceFile* file, VMEMArena* arena, bstr fname, DiagEngine* engine,
              IncludeDirVec idirs) {
  for (size_t i = 0; i < idirs.n; i++) {
    bstr dir = idirs.get[i];
    size_t dirlen = strlen(dir);
    size_t fnamelen = strlen(fname);
    // dir + '/' + fname + '\0'
    bstr path = vmarena_alloc(arena, dirlen + 1 + fnamelen + 1);
    memcpy(path, dir, dirlen);
    path[dirlen] = '/';
    memcpy(path + dirlen + 1, fname, fnamelen);
    path[dirlen + fnamelen + 1] = '\0';

    ScannerRes res = read_file(file, arena, fname);
    if (res == SE_OK)
      return;
  }
  throw_diag(engine, NULL_SPAN, AMAL_ERR_FILE_NOT_FOUND, fname);
}

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, InputFIleVec input_files,
                         jmp_buf* onerror) {
  DiagEngine engine = new_engine(amal_diaginfos, __amal_diaginfos_len, onerror);
  StringBuilder output = {};
  VMEMArenaMark mark = vmarena_mark(arena);
  for (size_t i = 0; i < input_files.n; i++) {
    SourceFile file;
    bstr fname = input_files.get[i];
    ScannerRes res = read_file(&file, arena, fname);
    if (res != SE_OK)
      try_find(&file, arena, fname, &engine, idirs);
    append_str(&output, file.contents);
    vmarena_reset(arena);
  }

  return output;
}
