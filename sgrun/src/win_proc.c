// NOTE: This is ripped-off code, please test this if you are on windows

#include "core/vec.h"
#include "sgrun/process.h"
#include "sgrun/win_proc.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct SGProcess {
  PROCESS_INFORMATION pi;
  bool has_status;
  DWORD status;
  HANDLE stdout_handle;
  HANDLE stderr_handle;
};

static int setup_capture(unsigned flags, unsigned flag, HANDLE* rd, HANDLE* wr) {
  *rd = NULL;
  *wr = NULL;

  if (!(flags & flag))
    return 0;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  if (!CreatePipe(rd, wr, &saAttr, 0))
    return -1;

  if (!SetHandleInformation(*rd, HANDLE_FLAG_INHERIT, 0)) {
    CloseHandle(*rd);
    CloseHandle(*wr);
    return -1;
  }

  return 0;
}

static void close_handle_safe(HANDLE* h) {
  if (*h && *h != INVALID_HANDLE_VALUE) {
    CloseHandle(*h);
    *h = NULL;
  }
}

SGProcess* win_spawn_proc(bstr exe_path, bstr const* argv, unsigned flags) {
  HANDLE stdout_rd = NULL, stdout_wr = NULL;
  HANDLE stderr_rd = NULL, stderr_wr = NULL;

  if (setup_capture(flags, SGPROC_CAPTURE_STDOUT, &stdout_rd, &stdout_wr) < 0)
    return NULL;

  if (setup_capture(flags, SGPROC_CAPTURE_STDERR, &stderr_rd, &stderr_wr) < 0) {
    close_handle_safe(&stdout_rd);
    close_handle_safe(&stdout_wr);
    return NULL;
  }

  char cmd[8192] = {0};
  for (size_t i = 0; argv[i]; i++) {
    if (i > 0)
      strcat(cmd, " ");

    bool needs_quotes = strchr(argv[i], ' ') || strchr(argv[i], '\t');
    if (needs_quotes)
      strcat(cmd, "\"");
    strcat(cmd, argv[i]);
    if (needs_quotes)
      strcat(cmd, "\"");
  }

  STARTUPINFOA si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (flags & (SGPROC_CAPTURE_STDOUT | SGPROC_CAPTURE_STDERR)) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = (flags & SGPROC_CAPTURE_STDOUT) ? stdout_wr : GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = (flags & SGPROC_CAPTURE_STDERR) ? stderr_wr : GetStdHandle(STD_ERROR_HANDLE);
  }

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  BOOL success =
      CreateProcessA(exe_path, cmd[0] ? cmd : NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

  close_handle_safe(&stdout_wr);
  close_handle_safe(&stderr_wr);

  if (!success) {
    close_handle_safe(&stdout_rd);
    close_handle_safe(&stderr_rd);
    return NULL;
  }

  SGProcess* proc = malloc(sizeof(SGProcess));
  if (!proc) {
    TerminateProcess(pi.hProcess, 1);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    close_handle_safe(&stdout_rd);
    close_handle_safe(&stderr_rd);
    return NULL;
  }

  proc->pi = pi;
  proc->has_status = false;
  proc->status = STILL_ACTIVE;
  proc->stdout_handle = stdout_rd;
  proc->stderr_handle = stderr_rd;

  CloseHandle(pi.hThread);
  return proc;
}

int win_wait_proc(SGProcess* proc) {
  DWORD res = WaitForSingleObject(proc->pi.hProcess, INFINITE);
  if (res == WAIT_OBJECT_0) {
    GetExitCodeProcess(proc->pi.hProcess, &proc->status);
    proc->has_status = true;
    return 1;
  }
  return 0;
}

int win_trywait_proc(SGProcess* proc) {
  DWORD res = WaitForSingleObject(proc->pi.hProcess, 0);
  if (res == WAIT_OBJECT_0) {
    GetExitCodeProcess(proc->pi.hProcess, &proc->status);
    proc->has_status = true;
    return 1;
  }
  return 0;
}

static bool pump_pipe_handle(HANDLE* h, StringBuilder* b) {
  if (!*h || *h == INVALID_HANDLE_VALUE)
    return false;

  while (true) {
    DWORD bytes_avail = 0;

    if (!PeekNamedPipe(*h, NULL, 0, NULL, &bytes_avail, NULL)) {
      close_handle_safe(h);
      return false;
    }

    if (bytes_avail == 0) {
      return true;
    }

    char buf[4096];
    DWORD bytes_read = 0;

    DWORD to_read = bytes_avail < sizeof(buf) ? bytes_avail : (DWORD)sizeof(buf);

    if (!ReadFile(*h, buf, to_read, &bytes_read, NULL) || bytes_read == 0) {
      close_handle_safe(h);
      return false;
    }

    StringView sv = {.str = buf, .len = (size_t)bytes_read};
    append_sv(b, sv);
  }
}

bool win_proc_pump_stdout(SGProcess* proc, StringBuilder* b) {
  return pump_pipe_handle(&proc->stdout_handle, b);
}

bool win_proc_pump_stderr(SGProcess* proc, StringBuilder* b) {
  return pump_pipe_handle(&proc->stderr_handle, b);
}

int win_kill_proc(SGProcess* proc) { return TerminateProcess(proc->pi.hProcess, 1) ? 1 : 0; }

void win_free_proc(SGProcess* proc) {
  if (!proc)
    return;

  close_handle_safe(&proc->stdout_handle);
  close_handle_safe(&proc->stderr_handle);
  CloseHandle(proc->pi.hProcess);
  free(proc);
}

SGProcessStatus win_proc_status(SGProcess* proc) {
  SGProcessStatus result;

  if (!proc->has_status) {
    DWORD code;
    if (GetExitCodeProcess(proc->pi.hProcess, &code)) {
      if (code == STILL_ACTIVE) {
        result.state = SGPROC_RUNNING;
        result.code = 0;
        return result;
      }
      proc->status = code;
      proc->has_status = true;
    }
  }

  result.state = SGPROC_EXITED;
  result.code = (int)proc->status;
  return result;
}
