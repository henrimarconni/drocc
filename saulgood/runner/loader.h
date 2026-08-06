#ifndef SG_LOADER_H
#define SG_LOADER_H

#include "core/stringdef.h"
#define SG_RUNNER_DEV
#include "sg_api.h"

/// Contains handles for lib
typedef struct {
  SGRuntime* saulg;
  struct SGTest* tests;
  void* handle;
  int tests_len;
  bstr name;
} SGTestLib;

/// Loads dynamic library and returns -1 on error
int sg_load(SGTestLib* lib, bstr name, bool show_output);
void sg_unload(SGTestLib* lib);


#endif
