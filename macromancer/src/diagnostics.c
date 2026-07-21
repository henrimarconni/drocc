#include "diagnostics.h"
#include "parser.h"
#include "span.h"
#include "stringdef.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <threads.h>

bstr errtype_to_msg[__error_type_len] = {
#define X(_, str) str,
    ERRORS(X)
#undef X
};

bstr notetype_to_msg[__note_type_len] = {
#define X(_, str) str,
    NOTES(X)
#undef X
};

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

[[noreturn]]
void _throw_error(Parser* p, Span span, ErrorType type, ...) {
  printf(ERROR_STR);
  va_list args;
  va_start(args, type);
  vformat(errtype_to_msg[type], args);
  va_end(args);
  putchar('\n');

  if (span.str)
    highlight_span(&p->file, span);
  longjmp(*p->onerror, -1);
}

void _add_note(Parser* p, NoteType type, ...) {
  printf(NOTE_STR);
  va_list args;
  va_start(args, type);
  vformat(notetype_to_msg[type], args);
  va_end(args);
  putchar('\n');
}
