#include "span.h"
#include "stringbuilder.h"

void append_span(StringBuilder* b, Span span) {
  while (span.len--) {
    vec_push(*b, *(span.str++));
  }
}

void append_str(StringBuilder* b, bstr str) {
  while (*str != '\0') {
    vec_push(*b, *(str++));
  }
}

void append_ch(StringBuilder* b, char ch) { vec_push(*b, ch); }
