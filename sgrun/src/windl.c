/// This file is conditionally included by cmake only on windows platform
// TODO: This is ripped-off code, please verify

#include "sgrun/windl.h"
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

void* windl_load(bstr name) { return (void*)LoadLibraryA(name); }

void windl_unload(void* handle) {
  if (handle)
    FreeLibrary((HMODULE)handle);
}

void* windl_get(void* handle, bstr name) {
  if (!handle)
    return NULL;
  return (void*)(uintptr_t)GetProcAddress((HMODULE)handle, name);
}

void windl_error() {
  DWORD err = GetLastError();
  char* msg = NULL;

  FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      err,
      0,
      (LPSTR)&msg,
      0,
      NULL);

  fprintf(stderr, "error %lu: %s\n", (unsigned long)err, msg ? msg : "unknown error");
  LocalFree(msg);
}
