#include "stringdef.h"
#include "utils.h"
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
  FILE* out = fopen(output_path, "w");
  if (!out) {
    // printf("Could not open output file %s for writing\n", output_path);
    return -1;
  }
  fprintf(out, "%s", str);
  if (output_path) {
    fclose(out);
  }
  return 0;
}
