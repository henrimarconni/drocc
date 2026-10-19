#include "core/cli_diag.h"
#include "core/diag_core.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

void clid_print_diag(CLIDiagEngine* eng, int error, ...) {
  va_list args;
  va_start(args, error);

  assert(eng->info_len > (size_t)error);
  DiagInfo info = eng->infos[error];

  print_diag_intro(info.level);
  diag_vprintf(info.msg, args);
  putchar('\n');
  va_end(args);
}

CLIDiagEngine clid_engine_new(const DiagInfo* infos, size_t info_len, jmp_buf* onerror) {
  CLIDiagEngine engine = {0};
  engine.infos = infos;
  engine.info_len = info_len;
  engine.onerror = onerror;
  return engine;
}

[[noreturn]]
void clid_throw_diag(CLIDiagEngine* eng, int error, ...) {
  va_list args;
  va_start(args, error);

  assert(eng->info_len > (size_t)error);
  DiagInfo info = eng->infos[error];

  print_diag_intro(info.level);
  diag_vprintf(info.msg, args);
  putchar('\n');
  va_end(args);
  longjmp(*eng->onerror, 1);
}
