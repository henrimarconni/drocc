#include "core/stringdef.h"
#include "runner.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

SGResult sg_run_process(bstr exec, int test_id) {
  pid_t pid = fork();

  if (pid < 0)
    return SG_ERROR;

  if (pid == 0) {
    saulgood_tests[test_id].fn();
    _exit(0);
  }

  int status;

  if (waitpid(pid, &status, 0) == -1)
    return SG_ERROR;

  if (WIFEXITED(status))
    return WEXITSTATUS(status) == 0 ? SG_PASS : SG_FAIL;

  if (WIFSIGNALED(status))
    return SG_CRASH;

  return SG_ERROR;
}
