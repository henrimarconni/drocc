#ifndef SG_LOADER_H
#define SG_LOADER_H

#include "core/stringdef.h"
#include <stdint.h>

#define SG_RUNNER_DEV
#include "sg_api.h"

/// Contains handles for lib
typedef struct {
  SGRuntime* saulg;
  struct SGTest* tests;
  void* handle;
  uint32_t tests_len;
  bstr name;
} SGTestLib;

typedef struct SGRunnerOptions SGRunnerOptions;

/// Loads dynamic library and uses clid_throw on error
void sg_load(SGRunnerOptions* rs, bstr name);
void sg_unload(SGTestLib* lib);


#endif
