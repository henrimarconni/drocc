#ifndef SG_FMT_H
#define SG_FMT_H

#include "core/stringdef.h"

#define HEADING_WIDTH 80U

PRINT_ATTR(2, 3)
void print_heading(char ch, bstr fmt, ...);

#endif
