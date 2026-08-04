#ifndef STRING_H
#define STRING_H

#include <stdint.h>

/// borrowed (dont free this)
typedef char* bstr;
/// owned (needed to be freed)
typedef char* ostr;

typedef struct {
  bstr str;
  uint32_t len;
} StringView;

#define strview(cstr) (StringView){ .len = strlen((cstr)), .str = (cstr) }

#endif
