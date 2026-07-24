#include "runner.h"
#include "stringdef.h"

#include <stdio.h>
#include <windows.h>

SGResult sg_run_process(bstr exec, int test_id) {
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "\"%s\" --test-id %d", exec, test_id);

  STARTUPINFOA si = {0};
  PROCESS_INFORMATION pi = {0};

  si.cb = sizeof(si);

  if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    return SG_ERROR;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exit_code;
  if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return SG_ERROR;
  }

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);

  return exit_code == 0 ? SG_PASS : SG_FAIL;
}
