#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include "stringdef.h"
#include "vec.h"
#include "span.h"

typedef vec(char) StringBuilder;

void append_span(StringBuilder* b, Span span);
void append_str(StringBuilder* b, bstr str);
void append_ch(StringBuilder* b, char ch);


#endif
