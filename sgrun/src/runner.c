#include "core/vmem_arena.h"
#include "sgrun/loader.h"
#include "sgrun/runner.h"
#include "sgrun/sg_api.h"
#include "sgrun/sg_fmt.h"
#include "sgrun/sg_scheduler.h"
#include "sgrun/testctx.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Run an individual test using -g option
static void sg_run_test(SGRunnerOptions* rs, uint32_t id) {
  if (id >= rs->lib.tests_len)
    return;

  struct SGTest test = rs->lib.tests[id];
  test.fn();
}

// Print summary at the end of the run
static void print_summary(SGTestLib* lib, size_t failed, size_t passed) {
  putchar('\n');
  print_heading('-', "Summary");
  printf("Library : %s\n", lib->name);
  printf("Total   : %d\n", lib->tests_len);
  printf("Passed  : %zu\n", passed);
  printf("Failed  : %zu\n", failed);

  if (failed == 0)
    printf("\nResult  : PASS\n");
  else
    printf("\nResult  : FAIL\n");

  putchar('\n');
}

// Run the full test library parallely
// Runs child processes underneath for isolation
static void sg_test_lib(SGRunnerOptions* rs) {
  size_t passed = 0;
  size_t failed = 0;
  SGJScheduler sched = sgjs_new(rs->max_jobs);

  print_heading('=', "Test Library: %s", rs->lib.name);
  printf("| Tests    |%6d |\n", rs->lib.tests_len);
  printf("| Max Jobs |%6d |\n", rs->max_jobs);
  printf("| Verbose  |%6s |\n\n", rs->show_output ? "true" : "false");
  int fmt_width = snprintf(NULL, 0, "%d", rs->lib.tests_len);

  // Submit all tests as tasks
  VMEMArena* arena = vmarena_new(128 * 1024);
  for (uint32_t i = 0; i < rs->lib.tests_len; i++) {
    SGTestCtx* ctx = vmarena_alloc(arena, sizeof(SGTestCtx));
    *ctx = new_test_ctx(i, &rs->lib.tests[i], rs, &passed, &failed, fmt_width);
    sgjs_submit(&sched, ctx, poll_test_ctx, start_new_test_ctx);
  }

  sgjs_await(&sched);

  sgjs_free(&sched);
  print_summary(&rs->lib, failed, passed);
  vmarena_free(arena);

  rs->success = !rs->success || failed ? false : true;
}

void execute_args(SGRunnerOptions* rs, CLIDiagEngine* clid) {
  if (rs->test_ids.n > 0) {
    for (uint32_t i = 0; i < rs->test_ids.n; i++) {
      sg_load(rs, rs->test_ids.get[i].name, clid);
      sg_run_test(rs, rs->test_ids.get[i].id);
      sg_unload(&rs->lib);
    }
    return;
  }

  for (uint32_t i = 0; i < rs->test_libs.n; i++) {
    sg_load(rs, rs->test_libs.get[i], clid);
    sg_test_lib(rs);
    sg_unload(&rs->lib);
  }
}
