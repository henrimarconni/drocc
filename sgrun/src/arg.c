#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/stringdef.h"
#include "core/strutils.h"
#include "core/vec.h"
#include "sgrun/diag.h"
#include "sgrun/loader.h"
#include "sgrun/runner.h"
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

static void parse_test_info(bstr str, int* tests_len, bstr* name, CLIDiagEngine* clid) {
  bstr tests_len_str;
  int res = split_str_inplace(str, ':', name, &tests_len_str);

  if (res < 0)
    clid_throw_diag(clid, SGR_CLI_RUN_FN_INVALID_CMD);

  int res2 = parse_int(tests_len, tests_len_str);

  if (res2 < 0)
    clid_throw_diag(clid, SGR_CLI_RUN_FN_INVALID_CMD);
}

void validate_args(SGRunnerOptions* rs, CLIDiagEngine* clid) {
  if (rs->test_ids.n > 0 && rs->test_libs.n != 0)
    clid_throw_diag(clid, SGR_CLI_EXTRA_LIB_SPECIFIED);
}

void free_sgrun_opts(SGRunnerOptions* rs) {
  vec_destroy(rs->test_ids);
  vec_destroy(rs->test_libs);
}

SGRunnerOptions sg_parse_args(char** argv, CLIDiagEngine* clid) {
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
      vec_push(rs.test_libs, popt.s);
      break;

    case 'o':
      rs.show_output = true;
      break;

    case 't':
      rs.timeout = (uint32_t)popt.d;
      break;

    case 'j':
      if (popt.d > 0)
        rs.max_jobs = (uint32_t)popt.d;
      break;

    case 'h':
      ce_printhelp();
      break;

    case 'g': {
      TestNameIDPair p = {0};
      parse_test_info(popt.s, (int*)&p.id, &p.name, clid);
      vec_push(rs.test_ids, p);
      break;
    }
    default:
      clid_throw_diag(clid, SGR_CLI_INVALID_ARG);
    }
  }

  return rs;
}
