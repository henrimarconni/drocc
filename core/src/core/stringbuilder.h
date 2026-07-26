#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include "core/stringdef.h"
#include "core/vec.h"
#include "core/span.h"

typedef vec(char) StringBuilder;

void append_span(StringBuilder* b, Span span);
void append_str(StringBuilder* b, bstr str);
void append_ch(StringBuilder* b, char ch);


#endif
