#ifndef SG_RUNNER_H
#define SG_RUNNER_H

#include "core/stringdef.h"
#include "loader.h"
#include <stddef.h>
#include <stdint.h>


typedef struct SGRunnerOptions {
  SGTestLib lib;
  bstr runner_exe;
  bool show_output;
  uint32_t max_jobs;
  uint32_t timeout;
} SGRunnerOptions;


typedef enum {
  SGRE_INVALID_ARG = -1,
  SGRE_CANT_OPEN_LIB = -2,
  SGRE_RUN_FN_INVALID_CMD = -3,
  SGRE_PROCESS_ERROR = -4
} SGRError;

void sg_run_test(SGRunnerOptions* rs, uint32_t id);
int sg_test_lib(SGRunnerOptions* rs);

#endif
