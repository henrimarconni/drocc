#ifndef SG_RUNNER_H
#define SG_RUNNER_H

#include "loader.h"
#include <stddef.h>


typedef struct {
  SGTestLib lib;
  bstr runner_exe;
} SGRunnerState;


typedef enum {
  SGRE_INVALID_ARG = -1,
  SGRE_CANT_OPEN_LIB = -2,
  SGRE_RUN_FN_INVALID_CMD = -3,
  SGRE_PROCESS_ERROR = -4
} SGRError;

SGRunnerState sg_new_runner(bstr name, bstr runner_exe);
void sg_runner_free(SGRunnerState* rs);
void sg_run_test(SGRunnerState* rs, size_t id);
void sg_test_lib(bstr name, bstr runner_exe, size_t max_jobs);

#endif
