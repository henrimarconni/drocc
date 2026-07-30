#include "posix_proc.h"
#include "process.h"
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct SGProcess {
  pid_t pid;
  int status;
  bool has_status;
};

SGProcess* posix_spawn_proc(bstr exe_path, bstr const* argv) {
  pid_t res = fork();
  if (res == -1)
    return NULL;

  if (res == 0) {
    execvp(exe_path, argv);
    _exit(127);
  }

  SGProcess* proc = malloc(sizeof(SGProcess));
  if (!proc)
    return NULL;

  proc->pid = res;
  proc->status = 0;
  proc->has_status = false;
  return proc;
}

int posix_wait_proc(SGProcess* proc) {
  int res = waitpid(proc->pid, &proc->status, 0);
  if (res > 0)
    proc->has_status = true;
  return res;
}

int posix_trywait_proc(SGProcess* proc) {
  int temp_status = 0;
  int res = waitpid(proc->pid, &temp_status, WNOHANG);

  if (res > 0) {
    proc->status = temp_status;
    proc->has_status = true;
  }

  return res;
}

int posix_kill_proc(SGProcess* proc) { return kill(proc->pid, SIGKILL); }

void posix_free_proc(SGProcess* proc) { free(proc); }

SGProcessStatus posix_proc_status(SGProcess* proc) {
  SGProcessStatus result;

  if (!proc->has_status) {
    result.state = SGPROC_RUNNING;
    result.code = 0;
    return result;
  }

  if (WIFEXITED(proc->status)) {
    result.state = SGPROC_EXITED;
    result.code = WEXITSTATUS(proc->status);
  } else if (WIFSIGNALED(proc->status)) {
    result.state = SGPROC_SIGNAL;
    result.code = WTERMSIG(proc->status);
  } else if (WIFSTOPPED(proc->status)) {
    result.state = SGPROC_STOPPED;
    result.code = WSTOPSIG(proc->status);
  } else {
    result.state = SGPROC_RUNNING;
    result.code = 0;
  }

  return result;
}
