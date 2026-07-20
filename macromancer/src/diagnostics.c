#include "diagnostics.h"
#include "parser.h"
#include "span.h"
#include "stringdef.h"
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <threads.h>

#define TAB_WIDTH 4

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

void print_n_spaces(int n) {
  while (n--)
    putchar(' ');
}

void print_line_start(int offset1) {
  print_n_spaces(offset1);
  printf("| ");
}

void highlight_span(Parser* p, Span span) {
  char line_no[32];
  int offset1 = snprintf(line_no, sizeof(line_no), "%zu ", span.row);

  print_line_start(offset1);
  putchar('\n');

  const char* line_start = span.str;
  while (line_start > p->source && line_start[-1] != '\n')
    --line_start;

  const char* line_end = span.str;
  while (*line_end && *line_end != '\n' && *line_end != '\r')
    ++line_end;

  printf("%s| ", line_no);
  for (const char* cur = line_start; cur < line_end;) {
    if (cur == span.str) {
      printf(ANSI_YELLOW "%.*s" ANSI_RESET, (int)span.len, span.str);
      cur += span.len;
      continue;
    }
    if (*cur == '\t')
      print_n_spaces(TAB_WIDTH);
    else
      putchar(*cur);
    cur++;
  }

  putchar('\n');

  print_line_start(offset1);
  for (const char* cur = line_start; cur < span.str; ++cur) {
    if (*cur == '\t')
      print_n_spaces(TAB_WIDTH);
    else
      putchar(' ');
  }

  putchar('^');
  for (size_t i = 1; i < span.len; ++i)
    putchar('~');
  putchar('\n');
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
    highlight_span(p, span);
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
