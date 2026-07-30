#include "process.h"
#include "win_proc.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// TODO: This is ripped-off code, please verify

struct SGProcess {
  PROCESS_INFORMATION pi;
  bool has_status;
  DWORD status;
};

SGProcess* win_spawn_proc(bstr exe_path, bstr const* argv) {
  STARTUPINFOA si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  /* Build command line */
  char cmd[4096] = {0};
  for (size_t i = 0; argv[i]; i++) {
    if (i)
      strcat(cmd, " ");
    strcat(cmd, argv[i]);
  }

  if (!CreateProcessA(exe_path, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    return NULL;

  SGProcess* proc = malloc(sizeof(SGProcess));
  if (!proc) {
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return NULL;
  }

  proc->pi = pi;
  proc->has_status = false;
  proc->status = STILL_ACTIVE;

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

int win_kill_proc(SGProcess* proc) { return TerminateProcess(proc->pi.hProcess, 1); }

void win_free_proc(SGProcess* proc) {
  CloseHandle(proc->pi.hProcess);
  free(proc);
}

SGProcessStatus win_proc_status(SGProcess* proc) {
  SGProcessStatus result;

  if (!proc->has_status) {
    DWORD code;
    if (GetExitCodeProcess(proc->pi.hProcess, &code) && code == STILL_ACTIVE) {
      result.state = SGPROC_RUNNING;
      result.code = 0;
      return result;
    }

    proc->status = code;
    proc->has_status = true;
  }

  result.state = SGPROC_EXITED;
  result.code = (int)proc->status;
  return result;
}
