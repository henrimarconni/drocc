#include "core/diagnostics.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

struct Diag {
  Span span;
  int diag_type;
};

static void print_str(bstr str) {
  while (*str)
    putchar(*str++);
}

static void print_sv(StringView sv) {
  while (sv.len--)
    putchar(*sv.str++);
}

static void vformat(bstr str, va_list args) {
  while (*str) {
    if (0)
      ;
#define X(fstr, fn, T)                                                                             \
  else if (strncmp(fstr, str, strlen(fstr)) == 0) {                                                \
    str += strlen(fstr);                                                                           \
    fn(va_arg(args, T));                                                                           \
  }
    FORMAT_SPECS(X)
#undef X
    else putchar(*str++);
  }
}

static void print_diag_intro(DiagLevel level) {
  switch (level) {
  case DL_ERROR:
    printf(ERROR_STR);
    break;
  case DL_NOTE:
    printf(NOTE_STR);
    break;
  }
}

DiagEngine
new_engine(const DiagInfo* infos, size_t info_len, SourceManager* sman, jmp_buf* onerror) {
  return (DiagEngine){infos, {}, info_len, onerror, sman};
}

[[noreturn]]
void _throw_diag(DiagEngine* eng, Span span, int type, ...) {
  assert(type < (int)eng->info_len);
  DiagInfo info = eng->infos[type];
  print_diag_intro(info.level);
  va_list args;
  va_start(args, type);
  vformat(info.msg, args);
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
  vformat(info.msg, args);
  va_end(args);
  putchar('\n');

  if (span.srcid != INVALID_SRC_ID)
    highlight_span(eng->sman, span);
}
