#include "capture.h"
#include "loader.h"
#include "platf_loader.h"

int sg_load(SGTestLib* lib, bstr name, bool show_output) {
  void* handle = sgdl_load(name, RTLD_LAZY);
  if (!handle)
    return -1;

  SGRuntime* saulg = sgdl_get(handle, "saulg");

  struct SGTest* tests = sgdl_get(handle, "saulgood_tests");
  int* tests_len = sgdl_get(handle, "sg_test_len");

  if (!tests || !saulg || !tests_len) {
    sgdl_unload(handle);
    return -1;
  }

  saulg->capture_begin = capture_begin;
  saulg->capture_end = capture_end;
  saulg->capture_discard = capture_discard;
  saulg->verbose = show_output;

  lib->name = name;
  lib->handle = handle;
  lib->saulg = saulg;
  lib->tests = tests;
  lib->tests_len = *tests_len;
  return 0;
}

void sg_unload(SGTestLib* lib) { sgdl_unload(lib->handle); }
