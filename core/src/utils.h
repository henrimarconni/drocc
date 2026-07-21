#ifndef UTILS_H
#define UTILS_H

#include "stringdef.h"

#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RESET "\x1b[0m"

bool file_exists(bstr file);

#endif
