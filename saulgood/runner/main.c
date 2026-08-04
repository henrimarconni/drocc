#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/stringdef.h"
#include "core/strutils.h"
#include "runner.h"
#include <assert.h>

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

int main(int argc, char** argv) {
  bstr runner_exe = argv[0];
  ce_initopt(argc, argv);
  ce_add_meta("sgrun", "SaulGood Test Runner", "./sgrun libtest1.so libtest2.so");
  ce_addopt("help", 'h', 0, "Print help");
  ce_addopt("run-fn", 'g', 's', "Runs a test from a specific library and exits");
  ce_addopt("jobs", 'j', 'd', "Specify maximum concurrent jobs");

  char ch;
  ParsedOpt popt;
  int max_jobs = DEFAULT_MAX_JOBS;

  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case CE_PLAIN_VALUE:
      sg_test_lib(popt.s, runner_exe, max_jobs);
      break;

    case 'j':
      if (popt.d > 0)
        max_jobs = popt.d;
      break;

    case 'h':
      ce_printhelp();
      break;

    case 'g': {
      bstr name;
      int test_id;
      parse_test_info(popt.s, &test_id, &name);

      SGRunnerState rs = sg_new_runner(name, runner_exe);
      sg_run_test(&rs, test_id);
      break;
    }
    default:
      clid_throw_diag(CLID_ERROR, SGRE_INVALID_ARG, "Invalid argument provided.");
    }
  }
}
