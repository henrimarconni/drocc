/**
  @file
  Utilities to parse string into different data types
*/

#ifndef STRPARSE_H
#define STRPARSE_H

#include "core/stringdef.h"


typedef enum {
  STRP_OK,
  STRP_INVALID_INT,
  STRP_INVALID_FLOAT
} StrParserError;


StrParserError parse_float(float* f, bstr str);
StrParserError parse_int(int* i, bstr str);



#endif
