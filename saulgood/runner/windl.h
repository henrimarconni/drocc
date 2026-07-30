/**
  @file
  Windows DLL Loader
*/

#ifndef SG_WIN_DL
#define SG_WIN_DL

#include "core/stringdef.h"

void* windl_load(bstr name);
void* windl_unload(void* handle);
void* windl_get(void* handle, bstr name);


#endif
