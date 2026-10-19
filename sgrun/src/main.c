#include "core/ce_getopt.h"
#include "core/signals.h"
#include "sgrun/diag.h"
#include "sgrun/loader.h"
#include "sgrun/runner.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static void handler(int sig) {
  printf("sgrun: caught signal %d", sig);
  exit(1);
}

int main(int argc, char** argv) {
  register_crash_handlers(handler);
  ce_initopt(argc, argv);
  ce_add_meta("sgrun", "SaulGood Test Runner", "./sgrun libtest1.so libtest2.so");
  ce_addopt("help", 'h', 0, "Print help");
  ce_addopt("run-fn", 'g', 's', "Runs a test from a specific library and exits");
  ce_addopt("jobs", 'j', 'd', "Specify maximum concurrent jobs");
  ce_addopt("show-output", 'p', 0, "Print the test output without capturing it");
  ce_addopt("timeout-ms", 't', 'd', "Set maximum timeout (in milliseconds)");

  jmp_buf onerror;
  SGRunnerOptions rs;

  if (setjmp(onerror) != 0) {
    free_sgrun_opts(&rs);
    ce_printhelp();
    exit(-1);
  }
  CLIDiagEngine clid = clid_engine_new(sgr_diaginfos, __sgr_diaginfos_len, &onerror);

  rs = sg_parse_args(argv, &clid);
  validate_args(&rs, &clid);
  execute_args(&rs, &clid);

  free_sgrun_opts(&rs);
}
