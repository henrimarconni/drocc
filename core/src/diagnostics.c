#include "core/diag_core.h"
#include "core/diagnostics.h"
#include "core/span.h"
#include "core/srcman.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

DiagEngine
new_engine(const DiagInfo* infos, size_t info_len, SourceManager* sman, jmp_buf* onerror) {
  return (DiagEngine){infos, info_len, onerror, sman};
}

[[noreturn]]
void _throw_diag(DiagEngine* eng, Span span, int type, ...) {
  assert(type < (int)eng->info_len);
  DiagInfo info = eng->infos[type];
  print_diag_intro(info.level);
  va_list args;
  va_start(args, type);
  diag_vprintf(info.msg, args);
  va_end(args);
  putchar('\n');

  if (span.srcid != INVALID_SRC_ID)
    highlight_span(eng->sman, span);
  longjmp(*eng->onerror, -1);
}

void _print_diag(DiagEngine* eng, Span span, int type, ...) {
  assert(type < (int)eng->info_len);
  DiagInfo info = eng->infos[type];
  print_diag_intro(info.level);
  va_list args;
  va_start(args, type);
  diag_vprintf(info.msg, args);
  va_end(args);
  putchar('\n');

  if (span.srcid != INVALID_SRC_ID)
    highlight_span(eng->sman, span);
}
