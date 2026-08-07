#include "core/stringbuilder.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "loader.h"
#include "platf_proc.h"
#include "platf_time.h"
#include "process.h"
#include "runner.h"
#include "sg_api.h"
#include "sg_fmt.h"
#include "sg_scheduler.h"
#include "sg_sleep.h"
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

/// Context for each job
typedef struct {
  struct SGTest* test;
  size_t id;
  SGProcess* proc;
  SGRunnerOptions* rs;
  size_t* passed;
  size_t* failed;
  StringBuilder err;
  StringBuilder out;
  int fmt_width;
  size_t time;
} SGTestCtx;

static SGTestCtx new_test_ctx(size_t id, struct SGTest* test, SGRunnerOptions* rs, size_t* passed,
                              size_t* failed, int fmt_width) {
  SGTestCtx ctx = {0};
  ctx.test = test;
  ctx.id = id;
  ctx.rs = rs;
  ctx.passed = passed;
  ctx.failed = failed;
  ctx.fmt_width = fmt_width;
  return ctx;
}

static bool start_new_job(void* ctx) {
  SGTestCtx* testctx = ctx;
  char buf[1024];
  snprintf(buf, sizeof(buf), "%s:%zu", testctx->rs->lib.name, testctx->id);

  // run in verbose mode
  if (testctx->rs->show_output) {
    testctx->proc = sg_spawn_proc(testctx->rs->runner_exe,
                                  (char*[]){testctx->rs->runner_exe, "-o", "-g", buf, NULL}, 0);
  }
  // run in normal mode
  else {
    testctx->proc =
        sg_spawn_proc(testctx->rs->runner_exe, (char*[]){testctx->rs->runner_exe, "-g", buf, NULL},
                      SGPROC_CAPTURE_STDERR | SGPROC_CAPTURE_STDOUT);
  }

  testctx->time = sgtime();
  return testctx->proc != NULL;
}

/// Print test status after test has finished and increase passed/failed number
void print_test(SGProcessStatus* status, SGTestCtx* ctx);

bool poll(void* ctx) {
  SGTestCtx* testctx = ctx;

  // Pump the pipes
  if (!testctx->rs->show_output) {
    sg_proc_pump_stderr(testctx->proc, &testctx->err);
    sg_proc_pump_stdout(testctx->proc, &testctx->out);
  }

  // Timeout: Kill the process and mark it as failed
  if (sgtime() - testctx->time > testctx->rs->timeout) {
    sg_kill_proc(testctx->proc);
    sleep_ms(500);
    SGProcessStatus status = sg_proc_status(testctx->proc);
    status.state = SGPROC_TIMEOUT;

    print_test(&status, testctx);
    sg_free_proc(testctx->proc);
    return false;
  }

  // Process stopped
  if (sg_trywait_proc(testctx->proc)) {
    SGProcessStatus status = sg_proc_status(testctx->proc);
    print_test(&status, testctx);
    sg_free_proc(testctx->proc);
    return false;
  }

  // continue
  return true;
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
    sgjs_submit(&sched, ctx, poll, start_new_job);
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

    if (err)
      vec_destroy(ctx->err);
    if (out)
      vec_destroy(ctx->out);
  }
}

void print_test(SGProcessStatus* status, SGTestCtx* ctx) {
  printf("[%*zu/%*d] %-40s", ctx->fmt_width, ctx->id + 1, ctx->fmt_width, ctx->rs->lib.tests_len,
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
