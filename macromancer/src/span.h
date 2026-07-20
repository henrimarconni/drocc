#ifndef SPAN_H
#define SPAN_H

#include "stringdef.h"
#include <stddef.h>
#include <stdbool.h>


typedef struct {
  size_t row, col, len;
  bstr str;
} Span;


bool span_cmp(Span span1, Span span2);
Span str_to_span(bstr str);
bool span_str_cmp(Span span, bstr str);
bstr dup_span_buf(Span span, bstr buf);
Span advance_span(Span span);

#endif
