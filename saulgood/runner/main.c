#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/stringdef.h"
#include "core/strparse.h"
#include "loader.h"
#include "sg_api.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  SGRE_INVALID_ARG = -1,
  SGRE_CANT_OPEN_LIB = -2,
  SGRE_RUN_FN_INVALID_CMD = -3,
} SGRErorr;

static bstr exe_path;

void sg_run_test(SGTestLib* lib, size_t id) {
  if (id >= lib->tests_len)
    return;
  struct SGTest test = lib->tests[id];
  printf("\nRunning test: %s\n", test.name);
  test.fn();
  printf("\nTest run successfully\n");
}

void sg_test_lib(bstr name) {
  SGTestLib lib;
  if (sg_load(&lib, name) < 0)
    clid_throw_diag(CLID_ERROR, SGRE_CANT_OPEN_LIB, "Cannot open library: %s", name);

  size_t i = 0;
  while (i < lib.tests_len) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s --run-fn %s:%zu", exe_path, lib.name, i++);
    system(buf);
  }

  sg_unload(&lib);
}

int main(int argc, char** argv) {
  exe_path = argv[0];
  ce_initopt(argc, argv);
  ce_add_meta("sgrun", "SaulGood Test Runner", "./sgrun libtest1.so libtest2.so");
  ce_addopt("help", 'h', 0, "Print help");
  ce_addopt("run-fn", 'g', 's', "Runs a test from a specific library and exits");

  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case CE_PLAIN_VALUE:
      sg_test_lib(popt.s);
      break;

    case 'h':
      ce_printhelp();
      break;

    case 'g': {
      bstr name = popt.s;

      while (*popt.s && *popt.s != ':')
        popt.s++;
      if (!*popt.s)
        clid_throw_diag(CLID_ERROR, SGRE_RUN_FN_INVALID_CMD, "Invalid arguments to --run-fn");
      *popt.s++ = '\0';

      int test_id;
      if (parse_int(&test_id, popt.s) != STRP_OK)
        clid_throw_diag(CLID_ERROR, SGRE_RUN_FN_INVALID_CMD, "Invalid arguments to --run-fn");
      assert(test_id >= 0);

      SGTestLib lib;
      sg_load(&lib, name);
      sg_run_test(&lib, test_id);
      sg_unload(&lib);

      break;
    }
    default:
      clid_throw_diag(CLID_ERROR, SGRE_INVALID_ARG, "Invalid argument provided.");
    }
  }
}
