#include "core/fs.h"
#include "core/stringdef.h"
#include <assert.h>
#include <stdio.h>

#if defined(_WIN32)
#include <io.h>
#define file_access(path) (_access((path), 0) == 0)
#else
#include <unistd.h>
#define file_access(path) (access((path), F_OK) == 0)
#endif

bool file_exists(bstr path) { return file_access(path); }

int write_out(bstr output_path, bstr str) {
  if (!output_path || !str)
    return -1;

  FILE* out = fopen(output_path, "w");
  if (!out)
    return -1;

  fputs(str, out);
  fclose(out);

  return 0;
}
