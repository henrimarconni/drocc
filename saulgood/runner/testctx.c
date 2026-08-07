#include "loader.h"
#include "platf_proc.h"
#include "platf_time.h"
#include "process.h"
#include "runner.h"
#include "sg_api.h"
#include "sg_sleep.h"
#include "testctx.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SGTestCtx new_test_ctx(size_t id, struct SGTest* test, SGRunnerOptions* rs, size_t* passed,
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

bool start_new_test_ctx(void* ctx) {
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

bool poll_test_ctx(void* ctx) {
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
