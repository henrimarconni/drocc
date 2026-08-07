/// This file is conditionally included by cmake only on windows platform
// TODO: This is ripped-off code, please verify

#include "windl.h"
#include <windows.h>

void* windl_load(bstr name) { return (void*)LoadLibraryA(name); }

void windl_unload(void* handle) {
  if (handle)
    FreeLibrary((HMODULE)handle);
}

void* windl_get(void* handle, bstr name) {
  if (!handle)
    return NULL;
  return (void*)GetProcAddress((HMODULE)handle, name);
}
