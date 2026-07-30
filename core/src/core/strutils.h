#ifndef STRUTILS_H
#define STRUTILS_H

#include "core/stringdef.h"

/**
  Splits string with the given delimeter and populates p1 and p2
  @note The input string is MODIFIED
  @return -1 if delimeter isnt found in string, 0 on success
*/
int split_str_inplace(bstr str, char delim, bstr* p1, bstr* p2);




#endif
