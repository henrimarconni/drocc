#include "capture.h"
#include "core/cli_diag.h"
#include "loader.h"
#include "platf_proc.h"
#include "process.h"
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
    snprintf(cmd, sizeof(cmd), "%s:%zu", lib.name, i);

    printf("[%*zu/%*d] %-40s ", fmt_width, i + 1, fmt_width, lib.tests_len, test.name);
    fflush(stdout);

    SGCapture out;
    SGCapture err;
    capture_begin(&out, stdout);
    capture_begin(&err, stderr);

    SGProcess* proc = sg_spawn_proc(runner_exe, (char*[]){runner_exe, "-g", cmd, NULL});
    if (sg_wait_proc(proc) < 0)
      clid_throw_diag(CLID_ERROR, SGRE_PROCESS_ERROR, "Cannot wait for process with test %s",
                      test.name);

    SGProcessStatus status = sg_proc_status(proc);
    if (status.state == SGPROC_EXITED && status.code == 0) {
      passed++;
      capture_discard(&out);
      capture_discard(&err);
      printf("PASS\n");
    } else {
      failed++;
      ostr output = capture_end(&out);
      ostr error = capture_end(&err);

      printf("FAIL (%d:%d)\n", status.state, status.code);

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
    sg_free_proc(proc);
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
