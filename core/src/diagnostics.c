#include "diagnostics.h"
#include "span.h"
#include "stringdef.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

struct Diag {
  Span span;
  int diag_type;
};

// TODO: Save diagnostics for later
// typedef enum { ST_STDOUT, ST_BUF } SinkType;

// typedef struct {
//   SinkType type;
//   union {
//     bstr buf;
//   };
// } DiagSink;

void print_str(bstr str) {
  while (*str)
    putchar(*str++);
}

void print_span(Span span) {
  while (span.len--)
    putchar(*span.str++);
}

void vformat(bstr str, va_list args) {
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

void print_diag_intro(DiagLevel level) {
  switch (level) {
  case DL_ERROR:
    printf(ERROR_STR);
    break;
  case DL_NOTE:
    printf(NOTE_STR);
    break;
  }
}

DiagEngine new_engine(DiagInfo* infos, size_t info_len, jmp_buf* onerror) {
  return (DiagEngine){infos, {}, info_len, onerror};
}

[[noreturn]]
void _throw_diag(DiagEngine* eng, Span span, int type, ...) {
  assert(type < eng->info_len);
  DiagInfo info = eng->infos[type];
  print_diag_intro(info.level);
  va_list args;
  va_start(args, type);
  vformat(info.msg, args);
  va_end(args);
  putchar('\n');

  if (span.str)
    highlight_span(span);
  longjmp(*eng->onerror, -1);
}

void _print_diag(DiagEngine* eng, Span span, int type, ...) {
  assert(type < eng->info_len);
  DiagInfo info = eng->infos[type];
  print_diag_intro(info.level);
  va_list args;
  va_start(args, type);
  vformat(info.msg, args);
  va_end(args);
  putchar('\n');

  if (span.str)
    highlight_span(span);
}
