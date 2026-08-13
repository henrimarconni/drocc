#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/signals.h"
#include "core/stringdef.h"
#include "core/strutils.h"
#include "sgrun/loader.h"
#include "sgrun/runner.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void parse_test_info(bstr str, int* tests_len, bstr* name) {
  bstr tests_len_str;
  int res = split_str_inplace(str, ':', name, &tests_len_str);

  if (res < 0)
    clid_throw_diag(CLID_ERROR, SGRE_RUN_FN_INVALID_CMD, "Invalid arguments to --run-fn");

  int res2 = parse_int(tests_len, tests_len_str);

  if (res2 < 0)
    clid_throw_diag(CLID_ERROR, SGRE_RUN_FN_INVALID_CMD, "Invalid arguments to --run-fn");
  if (tests_len < 0)
    clid_throw_diag(CLID_ERROR, SGRE_RUN_FN_INVALID_CMD, "Invalid arguments to --run-fn");
}

const int DEFAULT_MAX_JOBS = 2;
const size_t DEFAULT_MAX_TIMEOUT_MS = 10000;

void handler(int sig) {
  printf("sgrun: caught signal %d", sig);
  exit(1);
}

int main(int argc, char** argv) {
  register_crash_handlers(handler);
  bstr runner_exe = argv[0];
  ce_initopt(argc, argv);
  ce_add_meta("sgrun", "SaulGood Test Runner", "./sgrun libtest1.so libtest2.so");
  ce_addopt("help", 'h', 0, "Print help");
  ce_addopt("run-fn", 'g', 's', "Runs a test from a specific library and exits");
  ce_addopt("jobs", 'j', 'd', "Specify maximum concurrent jobs");
  ce_addopt("show-output", 'p', 0, "Print the test output without capturing it");
  ce_addopt("timeout-ms", 't', 'd', "Set maximum timeout (in milliseconds)");

  char ch;
  ParsedOpt popt;
  SGRunnerOptions rs = {0};
  rs.max_jobs = DEFAULT_MAX_JOBS;
  rs.show_output = false;
  rs.runner_exe = argv[0];
  rs.timeout = DEFAULT_MAX_TIMEOUT_MS;

  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case CE_PLAIN_VALUE:
      sg_load(&rs, popt.s);
      int res = sg_test_lib(&rs);
      sg_unload(&rs.lib);
      return res;

    case 'o':
      rs.show_output = true;
      break;

    case 't':
      rs.timeout = popt.d;
      break;

    case 'j':
      if (popt.d > 0)
        rs.max_jobs = popt.d;
      break;

    case 'h':
      ce_printhelp();
      break;

    case 'g': {
      bstr name;
      int test_id;
      parse_test_info(popt.s, &test_id, &name);
      sg_load(&rs, popt.s);
      sg_run_test(&rs, test_id);
      sg_unload(&rs.lib);
      return 0;
    }
    default:
      clid_throw_diag(CLID_ERROR, SGRE_INVALID_ARG, "Invalid argument provided.");
    }
  }
}
