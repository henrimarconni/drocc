#include "core/stringbuilder.h"

void append_sv(StringBuilder* b, StringView sv) {
  while (sv.len--) {
    vec_push(*b, *(sv.str++));
  }
}

void append_str(StringBuilder* b, bstr str) {
  while (*str != '\0') {
    vec_push(*b, *(str++));
  }
}

void append_ch(StringBuilder* b, char ch) { vec_push(*b, ch); }
