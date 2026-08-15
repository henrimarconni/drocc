#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#if defined(__GNUC__) || defined (__clang__)
#define PRINT_ATTR(a, b) __attribute__((format(printf, a, b)))
#else
#define PRINT_ATTR(a, b)
#endif

/// borrowed (dont free this)
typedef char* bstr;
/// owned (needed to be freed)
typedef char* ostr;

typedef struct {
  bstr str;
  uint32_t len;
} StringView;

/// true if equal
#define strview(cstr)                                                                              \
  (StringView) { .len = (uint32_t)strlen((cstr)), .str = (cstr) }

#endif
