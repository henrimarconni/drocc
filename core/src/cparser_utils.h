#ifndef CPARSER_UTILS_H
#define CPARSER_UTILS_H
#include "scanner.h"
#include "span.h"

void skip_c_comments(SourceFile* file, Span* span);
Span parse_cstr(SourceFile* file);

#endif
