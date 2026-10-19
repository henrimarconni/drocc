#include "core/diag_core.h"
#include <stdarg.h>
#include <stdio.h>

static void print_str(bstr str) {
  while (*str)
    putchar(*str++);
}

static void print_sv(StringView sv) {
  while (sv.len--)
    putchar(*sv.str++);
}

static void print_int(int i) { printf("%d", i); }

void diag_vprintf(bstr str, va_list args) {
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
  case DL_WARNING:
    printf(WARN_STR);
    break;
  case DL_NOTE:
    printf(NOTE_STR);
    break;
  }
}
