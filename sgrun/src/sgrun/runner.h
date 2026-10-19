#ifndef SG_RUNNER_H
#define SG_RUNNER_H

#include "core/cli_diag.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "loader.h"
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_MAX_JOBS 2
#define DEFAULT_MAX_TIMEOUT_MS 10000

typedef struct {
  bstr name;
  uint32_t id;
} TestNameIDPair;

typedef struct SGRunnerOptions {
  SGTestLib lib;
  bstr runner_exe;
  bool show_output;
  uint32_t max_jobs;
  uint32_t timeout;
  vec(bstr) test_libs;
  vec(TestNameIDPair) test_ids;
  bool success;
} SGRunnerOptions;

void free_sgrun_opts(SGRunnerOptions* rs);
SGRunnerOptions sg_parse_args(char** argv, CLIDiagEngine* clid);
void validate_args(SGRunnerOptions* rs, CLIDiagEngine* clid);
void execute_args(SGRunnerOptions* rs, CLIDiagEngine* clid);

#endif
