#ifndef SPAN_H
#define SPAN_H

#include "scanner.h"
#include "stringdef.h"
#include <stddef.h>
#include <stdbool.h>

#define NULL_SPAN (Span){0}
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RESET "\x1b[0m"

typedef struct {
  size_t row, col, len;
  bstr str;
  SourceFile* file;
} Span;


Span span_begin(SourceFile *file);
void span_end(Span *span);
void highlight_span(Span span);
bool span_cmp(Span span1, Span span2);
Span str_to_span(bstr str);
bool span_str_cmp(Span span, bstr str);
bstr dup_span_buf(Span span, bstr buf);
void advance_span(Span* span);
void shrink_span(Span* span);

#endif
