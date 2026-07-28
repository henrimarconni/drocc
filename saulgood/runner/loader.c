#include "capture.h"
#include "loader.h"
#include <dlfcn.h>

int sg_load(SGTestLib* lib, bstr name) {
  void* handle = dlopen(name, RTLD_LAZY);
  if (!handle)
    return -1;

  SGRuntime* saulg = dlsym(handle, "saulg");

  struct SGTest* tests = dlsym(handle, "saulgood_tests");
  int* tests_len = dlsym(handle, "sg_test_len");

  if (!tests || !saulg || !tests_len) {
    dlclose(handle);
    return -1;
  }

  saulg->capture_begin = capture_begin;
  saulg->capture_end = capture_end;

  // printf("tests[%d] = %p\n", *tests_len, (void*)tests);
  // printf("fn        = %p\n", (void*)tests[0].fn);
  // printf("name      = %s\n", tests[0].name);

  lib->name = name;
  lib->handle = handle;
  lib->saulg = saulg;
  lib->tests = tests;
  lib->tests_len = *tests_len;
  return 0;
}

void sg_unload(SGTestLib* lib) { dlclose(lib->handle); }
