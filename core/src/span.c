#include "core/span.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define TAB_WIDTH 4

static inline bstr span_str(SourceManager* man, Span span) {
  return &man->sources.get[span.srcid].contents[span.offset];
}

StringView span_sv(SourceManager* man, Span span) {
  return (StringView){span_str(man, span), span.len};
}

bool span_cmp(Span span1, Span span2) { return memcmp(&span1, &span2, sizeof(Span)) == 0; }

bool span_str_cmp(SourceManager* man, Span span, bstr str) {
  bstr str2 = span_str(man, span);
  while (*str && span.len) {
    if (*str != *str2)
      return false;
    span.len--;
    str2++;
    str++;
  }
  return true;
}

// Gives null terminated duplicate
bstr dup_span_buf(SourceManager* man, Span span, bstr buf, size_t size) {
  assert(size > span.len);
  bstr str = span_str(man, span);
  memcpy(buf, str, span.len);
  buf[span.len] = '\0';
  return buf;
}

void print_n_spaces(int n) {
  while (n--)
    putchar(' ');
}

void print_line_start(int offset1) {
  print_n_spaces(offset1);
  printf("| ");
}

void highlight_span(SourceManager* man, Span span) {
  SMSpanInfo info = sman_info(man, span);

  char line_no[32];
  int offset1 = snprintf(line_no, sizeof(line_no), "%d ", info.row);

  // line 1
  print_line_start(offset1);
  putchar('\n');

  // line 2, with actual contents
  putchar('\n');
  bstr line_start = info.sv.str - info.col;
  size_t spaces = 0;

  // print line
  for (bstr cur = line_start; *cur && *cur != '\n'; cur++) {
    // highlight the span
    if (cur == info.sv.str)
      printf(ANSI_YELLOW);
    // reset the color back
    else if (cur == info.sv.str + info.sv.len)
      printf(ANSI_RESET);

    // replace \t with TAB_WIDTH spaces
    if (*cur == '\t') {
      print_n_spaces(TAB_WIDTH);
      spaces += TAB_WIDTH;
    }
    // else just print the character
    else {
      putchar(*cur);
      spaces++;
    }
  }

  // line 3, spaces before span start + ^~~~ highlighting
  print_line_start(offset1);
  print_n_spaces(spaces);
  putchar('^');
  for (size_t i = 1; i < span.len; ++i)
    putchar('~');
  putchar('\n');
}
