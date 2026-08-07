/**
  @file
  Posix DLL Loader
*/

#ifndef SG_POSIX_DL
#define SG_POSIX_DL

#include "core/stringdef.h"

void* posixdl_load(bstr name);
void posixdl_unload(void* handle);
void* posixdl_get(void* handle, bstr name);


#endif
