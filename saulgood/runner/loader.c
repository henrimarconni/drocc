#include "capture.h"
#include "core/cli_diag.h"
#include "loader.h"
#include "platf_loader.h"
#include "runner.h"

void sg_load(SGRunnerOptions* rs, bstr name) {
  void* handle = sgdl_load(name);
  if (!handle)
    goto throw;

  SGRuntime* saulg = sgdl_get(handle, "saulg");

  struct SGTest* tests = sgdl_get(handle, "saulgood_tests");
  int* tests_len = sgdl_get(handle, "sg_test_len");

  if (!tests || !saulg || !tests_len) {
    sgdl_unload(handle);
    goto throw;
  }

  saulg->capture_begin = capture_begin;
  saulg->capture_end = capture_end;
  saulg->capture_discard = capture_discard;
  saulg->verbose = rs->show_output;

  rs->lib.name = name;
  rs->lib.handle = handle;
  rs->lib.saulg = saulg;
  rs->lib.tests = tests;
  rs->lib.tests_len = *tests_len;
  return;

  throw : clid_throw_diag(CLID_ERROR, SGRE_CANT_OPEN_LIB, "Cannot open test library %s", name);
}

void sg_unload(SGTestLib* lib) { sgdl_unload(lib->handle); }
