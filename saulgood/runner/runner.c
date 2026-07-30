#include "core/cli_diag.h"
#include "core/vmem_arena.h"
#include "loader.h"
#include "platf_proc.h"
#include "process.h"
#include "runner.h"
#include "sg_api.h"
#include "sg_fmt.h"
#include "sg_scheduler.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Create the state
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

/// This is run by the -g option and not directly
void sg_run_test(SGRunnerState* rs, size_t id) {
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
  SGRunnerState* rs;
} SGTestCtx;

static SGTestCtx new_test_ctx(size_t id, struct SGTest* test, SGRunnerState* rs) {
  SGTestCtx ctx = {0};
  ctx.test = test;
  ctx.id = id;
  ctx.rs = rs;
  return ctx;
}

static bool start_new_job(SGTestCtx* ctx) {
  char buf[1024];
  snprintf(buf, sizeof(buf), "%s:%zu", ctx->rs->lib.name, ctx->id);
  ctx->proc = sg_spawn_proc(ctx->rs->runner_exe, (char*[]){ctx->rs->runner_exe, "-g", buf, NULL},
                            SGPROC_CAPTURE_STDERR | SGPROC_CAPTURE_STDOUT);
  return ctx->proc != NULL;
}

/// Print test status after test has finished
bool print_test(int fmt_width, SGTestCtx* ctx) {
  printf("[%*zu/%*d] %-40s", fmt_width, ctx->id + 1, fmt_width, ctx->rs->lib.tests_len,
         ctx->test->name);

  SGProcessStatus status = sg_proc_status(ctx->proc);
  bool passed = false;

  if (status.state == SGPROC_EXITED && status.code == 0) {
    printf("PASS\n");
    passed = true;
  } else {
    printf("FAIL (%d:%d)\n", status.state, status.code);
    ostr err = sg_proc_take_stderr(ctx->proc);
    ostr out = sg_proc_take_stdout(ctx->proc);

    if (err && strlen(err) > 0)
      printf("stderr:\n%s\n", err);
    if (out && strlen(out) > 0)
      printf("stdout:\n%s\n", out);

    if (err)
      free(err);
    if (out)
      free(out);
  }

  return passed;
}

/// Run the full test library parallely
/// Runs child processes underneath for isolation
void sg_test_lib(bstr name, bstr runner_exe, size_t max_jobs) {

  // Initialise
  size_t passed = 0;
  size_t failed = 0;

  SGRunnerState rs = sg_new_runner(name, runner_exe);
  SGJScheduler sched = sgjs_new(max_jobs);

  // Print heading
  print_heading('=', "Test Library: %s", rs.lib.name);
  printf("Tests   : %d\n", rs.lib.tests_len);
  printf("Max Jobs: %zu\n\n", max_jobs);
  int fmt_width = snprintf(NULL, 0, "%d", rs.lib.tests_len);

  // Submit all tests as tasks
  VMEMArena* arena = vmarena_new(128 * 1024);
  for (size_t i = 0; i < rs.lib.tests_len; i++) {
    SGTestCtx tmpctx = new_test_ctx(i, &rs.lib.tests[i], &rs);
    SGTestCtx* ctx = vmarena_alloc(arena, sizeof(SGTestCtx));
    *ctx = tmpctx;
    sgjs_submit(&sched, (SGJob){ctx, SGJS_STARTING});
  }

  // Poll jobs
  SGJob* job;
  while ((job = sgjs_poll(&sched)) != NULL) {
    SGTestCtx* ctx = job->data;
    switch (job->state) {

    // If a new job is starting, start the test process
    case SGJS_STARTING: {
      if (!start_new_job(ctx))
        sgjs_stop(&sched, job);
      break;
    }

    // Check if process exited, and if so, print test status and cleanup
    case SGJS_RUNNING: {
      if (sg_trywait_proc(ctx->proc)) {
        if (print_test(fmt_width, ctx))
          passed++;
        else
          failed++;

        sgjs_stop(&sched, job);
        sg_free_proc(ctx->proc);
      }
      break;
    }

    default:
      assert(false);
    }
  }

  sgjs_free(&sched);
  print_summary(&rs.lib, failed, passed);
  vmarena_free(arena);
}
