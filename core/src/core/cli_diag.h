#ifndef CLI_DIAG_H_
#define CLI_DIAG_H_

#include "core/diag_core.h"
#include <setjmp.h>

typedef struct {
  const DiagInfo* infos;
  size_t info_len;
  jmp_buf* onerror;
} CLIDiagEngine;


CLIDiagEngine clid_engine_new(const DiagInfo* infos, size_t info_len, jmp_buf* onerror);
void clid_print_diag(CLIDiagEngine* eng, int error, ...);

[[noreturn]]
void clid_throw_diag(CLIDiagEngine* eng, int error, ...);

#endif
