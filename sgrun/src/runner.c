#include "core/vec.h"
#include "core/vmem_arena.h"
#include "sgrun/loader.h"
#include "sgrun/process.h"
#include "sgrun/runner.h"
#include "sgrun/sg_api.h"
#include "sgrun/sg_fmt.h"
#include "sgrun/sg_scheduler.h"
#include "sgrun/testctx.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// This is run by the -g option and not directly
void sg_run_test(SGRunnerOptions* rs, size_t id) {
  if (id >= rs->lib.tests_len)
    return;

  struct SGTest test = rs->lib.tests[id];
  test.fn();
}

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

/// Run the full test library parallely
/// Runs child processes underneath for isolation
int sg_test_lib(SGRunnerOptions* rs) {
  size_t passed = 0;
  size_t failed = 0;
  SGJScheduler sched = sgjs_new(rs->max_jobs);

  print_heading('=', "Test Library: %s", rs->lib.name);
  printf("| Tests    |%6d |\n", rs->lib.tests_len);
  printf("| Max Jobs |%6zu |\n", rs->max_jobs);
  printf("| Verbose  |%6s |\n\n", rs->show_output ? "true" : "false");
  int fmt_width = snprintf(NULL, 0, "%d", rs->lib.tests_len);

  // Submit all tests as tasks
  VMEMArena* arena = vmarena_new(128 * 1024);
  for (size_t i = 0; i < rs->lib.tests_len; i++) {
    SGTestCtx* ctx = vmarena_alloc(arena, sizeof(SGTestCtx));
    *ctx = new_test_ctx(i, &rs->lib.tests[i], rs, &passed, &failed, fmt_width);
    sgjs_submit(&sched, ctx, poll_test_ctx, start_new_test_ctx);
  }

  sgjs_await(&sched);

  sgjs_free(&sched);
  print_summary(&rs->lib, failed, passed);
  vmarena_free(arena);

  // 1 if failed, 0 if passed
  return failed > 0;
}

void print_failed_output(SGTestCtx* ctx) {
  // print captured output only when --show-output isnt set and theres a failure
  // else it prints everything automatically
  if (!ctx->rs->show_output) {
    ostr err = ctx->err.get;
    ostr out = ctx->out.get;

    if (err && strlen(err) > 0)
      printf("stderr:\n%s\n", err);
    if (out && strlen(out) > 0)
      printf("stdout:\n%s\n", out);
  }
}

void print_test(SGProcessStatus* status, SGTestCtx* ctx) {
  printf(
      "[%*zu/%*d] %-40s",
      ctx->fmt_width,
      ctx->id + 1,
      ctx->fmt_width,
      ctx->rs->lib.tests_len,
      ctx->test->name);

  if (status->state == SGPROC_TIMEOUT) {
    (*ctx->failed)++;
    printf("KILLED (timeout)\n");
    print_failed_output(ctx);
  } else if (status->state == SGPROC_EXITED && status->code == 0) {
    printf("PASS\n");
    (*ctx->passed)++;
  } else {
    (*ctx->failed)++;
    printf("FAIL (%d:%d)\n", status->state, status->code);
    print_failed_output(ctx);
  }
}
