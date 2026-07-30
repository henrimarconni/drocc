#include "core/strparse.h"
#include <ctype.h>

StrParserError parse_float(float* f, bstr str) {
  *f = 0;
  bool floated = false;
  float divisor = 1;
  char ch;
  while ((ch = *str++) != '\0') {
    if (ch == '.') {
      if (floated)
        return STRP_INVALID_FLOAT;
      floated = true;
      continue;
    }
    if (!isdigit(ch))
      return STRP_INVALID_FLOAT;
    if (!floated) {
      *f = (*f * 10.0) + (ch - '0');
    } else {
      divisor *= 10.0;
      *f = *f + ((ch - '0') / divisor);
    }
  }
  return STRP_OK;
}

StrParserError parse_int(int* num, bstr str) {
  char ch;
  int mul = 1;
  *num = 0;

  if (*str == '-') {
    mul = -1;
    str++;
  }

  while ((ch = *str++) != '\0') {
    if (!isdigit(ch))
      return STRP_INVALID_INT;
    *num *= 10;
    *num += ch - '0';
  }

  *num *= mul;
  return STRP_OK;
}
