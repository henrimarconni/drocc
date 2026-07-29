#include "capture.h"
#include "core/cli_diag.h"
#include "loader.h"
#include "runner.h"
#include "sg_api.h"
#include "sg_fmt.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SGRunnerState sg_new_runner(bstr name, bstr runner_exe) {
  SGTestLib lib;
  if (sg_load(&lib, name) < 0)
    clid_throw_diag(CLID_ERROR, SGRE_CANT_OPEN_LIB, "Cannot open library: %s", name);

  SGRunnerState rs;
  rs.runner_exe = runner_exe;
  rs.lib = lib;
  return rs;
}

void sg_runner_free(SGRunnerState* rs) { sg_unload(&rs->lib); }

void sg_run_test(SGRunnerState* rs, size_t id) {
  if (id >= rs->lib.tests_len)
    return;

  struct SGTest test = rs->lib.tests[id];
  test.fn();
}

void sg_test_lib(bstr name, bstr runner_exe) {
  SGTestLib lib;
  if (sg_load(&lib, name) < 0)
    clid_throw_diag(CLID_ERROR, SGRE_CANT_OPEN_LIB, "Cannot open library: %s", name);

  size_t passed = 0;
  size_t failed = 0;

  print_heading('=', "Test Library: %s", lib.name);
  printf("Tests: %d\n\n", lib.tests_len);
  int fmt_width = snprintf(NULL, 0, "%d", lib.tests_len);

  for (size_t i = 0; i < lib.tests_len; i++) {
    struct SGTest test = lib.tests[i];

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "%s --run-fn %s:%zu", runner_exe, lib.name, i);

    printf("[%*zu/%*d] %-40s ", fmt_width, i + 1, fmt_width, lib.tests_len, test.name);
    fflush(stdout);

    SGCapture out;
    SGCapture err;
    capture_begin(&out, stdout);
    capture_begin(&err, stderr);

    int status = system(cmd);

    if (status == 0) {
      passed++;
      capture_discard(&out);
      capture_discard(&err);
      printf("PASS\n");
    } else {
      failed++;
      ostr output = capture_end(&out);
      ostr error = capture_end(&err);

      printf("FAIL (%d)\n", status);

      if (strlen(output) != 0) {
        printf("stdout: \n");
        printf("%s\n", output);
      }
      if (strlen(error) != 0) {
        printf("stderr: \n");
        printf("%s\n", error);
      }

      free(output);
      free(error);
    }
  }

  putchar('\n');

  print_heading('-', "Summary");

  printf("Library : %s\n", lib.name);
  printf("Total   : %d\n", lib.tests_len);
  printf("Passed  : %zu\n", passed);
  printf("Failed  : %zu\n", failed);

  if (failed == 0)
    printf("\nResult  : PASS\n");
  else
    printf("\nResult  : FAIL\n");

  putchar('\n');

  sg_unload(&lib);
}
