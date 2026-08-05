#ifndef STRUTILS_H
#define STRUTILS_H

#include "core/stringdef.h"
#include <stdbool.h>

/**
  Splits string with the given delimeter and populates p1 and p2
  @note The input string is MODIFIED
  @return -1 if delimeter isnt found in string, 0 on success
*/
int split_str_inplace(bstr str, char delim, bstr* p1, bstr* p2);

bool sv_cmp(StringView s1, StringView s2);


/// @return -1 on failure
int parse_float(float* f, bstr str);
/// @return -1 on failure
int parse_int(int* i, bstr str);




#endif
