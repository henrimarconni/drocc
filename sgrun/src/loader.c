#include "core/cli_diag.h"
#include "platf_loader.h"
#include "sgrun/capture.h"
#include "sgrun/loader.h"
#include "sgrun/runner.h"
#include <stdint.h>

void sg_load(SGRunnerOptions* rs, bstr name) {
  void* handle = sgdl_load(name);
  if (!handle)
    goto throw;

  SGRuntime* saulg = sgdl_get(handle, "saulg");

  struct SGTest* tests = sgdl_get(handle, "saulgood_tests");
  uint32_t* tests_len = sgdl_get(handle, "sg_test_len");

  if (!tests || !saulg || !tests_len) {
    sgdl_unload(handle);
    goto throw;
  }

  saulg->capture_begin = capture_begin;
  saulg->capture_end = capture_end;
  saulg->capture_discard = capture_discard;

  rs->lib.name = name;
  rs->lib.handle = handle;
  rs->lib.saulg = saulg;
  rs->lib.tests = tests;
  rs->lib.tests_len = *tests_len;
  return;

  throw : clid_throw_diag(CLID_ERROR, SGRE_CANT_OPEN_LIB, "Cannot open test library %s", name);
}

void sg_unload(SGTestLib* lib) { sgdl_unload(lib->handle); }
