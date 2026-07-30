#include "core/strutils.h"

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
