/**
  @file
  Simple StringBuilder based on core/vec.h
*/

#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include "core/stringdef.h"
#include "core/vec.h"

typedef vec(char) StringBuilder;

void append_sv(StringBuilder* b, StringView sv);
void append_str(StringBuilder* b, bstr str);
void append_ch(StringBuilder* b, char ch);


#endif
