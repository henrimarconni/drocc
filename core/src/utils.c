#include "stringdef.h"
#include "utils.h"
#include <assert.h>

#if defined(_WIN32)
#include <io.h>
#define file_access(path) (_access((path), 0) == 0)
#else
#include <unistd.h>
#define file_access(path) (access((path), F_OK) == 0)
#endif

bool file_exists(bstr path) { return file_access(path); }
