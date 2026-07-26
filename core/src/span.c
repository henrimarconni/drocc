#include "core/scanner.h"
#include "core/span.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define TAB_WIDTH 4

bool span_cmp(Span span1, Span span2) {
  return span1.len == span2.len &&
         (span1.str == span2.str || memcmp(span1.str, span2.str, span1.len) == 0);
}

Span str_to_span(bstr str) { return (Span){0, 0, strlen(str), str, NULL}; }

bool span_str_cmp(Span span, bstr str) {
  if (span.len != strlen(str))
    return false;
  while (*str && span.len > 0) {
    if (*span.str != *str)
      return false;
    span.str++;
    span.len--;
    str++;
  }
  return true;
}

// Gives null terminated duplicate
bstr dup_span_buf(Span span, bstr buf) {
  memcpy(buf, span.str, span.len);
  buf[span.len] = '\0';
  return buf;
}

void advance_span(Span* span) {
  if (span->len > 0) {
    span->str++;
    span->len--;
  }
}

void shrink_span(Span* span) {
  assert(span->len > 0);
  span->len--;
}

void extend_span(Span* span) {
  assert(span->len + span->str <= span->file->contents + span->file->pos.len);
  nextch(span->file);
  span->len++;
}

Span span_begin(SourceFile* file) {
  return (Span){.str = &file->contents[file->pos.id],
                .len = 0,
                .row = file->pos.row,
                .col = file->pos.col,
                .file = file};
}

void span_end(Span* span) { span->len = &span->file->contents[span->file->pos.id] - span->str; }

void print_n_spaces(int n) {
  while (n--)
    putchar(' ');
}

void print_line_start(int offset1) {
  print_n_spaces(offset1);
  printf("| ");
}

void highlight_span(Span span) {
  char line_no[32];
  int offset1 = snprintf(line_no, sizeof(line_no), "%zu ", span.row);

  print_line_start(offset1);
  putchar('\n');

  const char* line_start = span.str;
  while (line_start > span.file->contents && line_start[-1] != '\n')
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
