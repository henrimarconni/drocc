#include "core/strutils.h"
#include <ctype.h>
#include <string.h>

int parse_float(float* f, bstr str) {
  *f = 0;
  bool floated = false;
  float divisor = 1;
  char ch;

  while ((ch = *str++) != '\0') {
    if (ch == '.') {
      if (floated)
        return -1;
      floated = true;
      continue;
    }

    if (!isdigit(ch))
      return -1;

    if (!floated)
      *f = (*f * 10.0) + (ch - '0');
    else {
      divisor *= 10.0;
      *f = *f + ((ch - '0') / divisor);
    }
  }
  return 0;
}

int parse_int(int* num, bstr str) {
  char ch;
  int mul = 1;
  *num = 0;

  if (*str == '-') {
    mul = -1;
    str++;
  }

  while ((ch = *str++) != '\0') {
    if (!isdigit(ch))
      return -1;
    *num *= 10;
    *num += ch - '0';
  }

  *num *= mul;
  return 0;
}

int split_str_inplace(bstr str, char delim, bstr* p1, bstr* p2) {
  *p1 = str;

  while (*str != '\0' && *str != delim)
    str++;
  if (*str == '\0')
    return -1;

  *str++ = '\0'; // skip delim and set it to '\0'
  *p2 = str;
  return 0;
}

bool sv_cmp(StringView s1, StringView s2) {
  return s1.len == s2.len && memcmp(s1.str, s2.str, s1.len) == 0;
}
