#include "core/vec.h"
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
  int stdout_fd;
  int stderr_fd;
};

static int setup_capture(unsigned flags, unsigned flag, int fds[2]) {
  fds[0] = fds[1] = -1;

  if (!(flags & flag))
    return 0;

  return pipe(fds);
}

static void child_capture(unsigned flags, unsigned flag, int fds[2], int target_fd) {
  if (!(flags & flag))
    return;

  close(fds[0]);
  dup2(fds[1], target_fd);
  close(fds[1]);
}

static void parent_capture(unsigned flags, unsigned flag, int fds[2], int* out_fd) {
  *out_fd = -1;

  if (!(flags & flag))
    return;

  close(fds[1]);
  *out_fd = fds[0];
}

static void close_capture(int fds[2]) {
  if (fds[0] != -1)
    close(fds[0]);
  if (fds[1] != -1)
    close(fds[1]);
}

SGProcess* posix_spawn_proc(bstr exe_path, bstr const* argv, unsigned flags) {
  int stdout_fds[2];
  int stderr_fds[2];

  if (setup_capture(flags, SGPROC_CAPTURE_STDOUT, stdout_fds) < 0)
    return NULL;

  if (setup_capture(flags, SGPROC_CAPTURE_STDERR, stderr_fds) < 0) {
    close_capture(stdout_fds);
    return NULL;
  }

  pid_t pid = fork();
  if (pid < 0) {
    close_capture(stdout_fds);
    close_capture(stderr_fds);
    return NULL;
  }

  if (pid == 0) {
    child_capture(flags, SGPROC_CAPTURE_STDOUT, stdout_fds, STDOUT_FILENO);
    child_capture(flags, SGPROC_CAPTURE_STDERR, stderr_fds, STDERR_FILENO);

    execvp(exe_path, argv);
    _exit(127);
  }

  SGProcess* proc = malloc(sizeof(SGProcess));
  if (!proc) {
    close_capture(stdout_fds);
    close_capture(stderr_fds);
    return NULL;
  }

  parent_capture(flags, SGPROC_CAPTURE_STDOUT, stdout_fds, &proc->stdout_fd);
  parent_capture(flags, SGPROC_CAPTURE_STDERR, stderr_fds, &proc->stderr_fd);

  proc->pid = pid;
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

static ostr take_pipe(int* fd) {
  if (*fd == -1)
    return NULL;

  vec(char) out = {0};

  char buf[4096];
  ssize_t n;

  while ((n = read(*fd, buf, sizeof(buf))) > 0) {
    while (out.n + (size_t)n + 1 > out.m)
      vec_grow(out);

    memcpy(out.get + out.n, buf, (size_t)n);
    out.n += (size_t)n;
  }

  close(*fd);
  *fd = -1;

  vec_push(out, '\0');
  return out.get;
}

ostr posix_proc_take_stdout(SGProcess* proc) { return take_pipe(&proc->stdout_fd); }

ostr posix_proc_take_stderr(SGProcess* proc) { return take_pipe(&proc->stderr_fd); }

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
