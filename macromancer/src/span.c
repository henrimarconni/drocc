#include "span.h"
#include <stdbool.h>
#include <string.h>

bool span_cmp(Span span1, Span span2) {
  return span1.len == span2.len &&
         (span1.str == span2.str || memcmp(span1.str, span2.str, span1.len) == 0);
}

Span str_to_span(bstr str) { return (Span){0, 0, strlen(str), str}; }

bool span_str_cmp(Span span, bstr str) {
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

Span advance_span(Span span) {
  if (span.len > 0) {
    span.str++;
    span.len--;
  }
  return span;
}
