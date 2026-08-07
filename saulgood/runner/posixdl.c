#include "posixdl.h"
#include <dlfcn.h>

void* posixdl_load(bstr name) { return dlopen(name, RTLD_LAZY); }
void posixdl_unload(void* handle) { dlclose(handle); }
void* posixdl_get(void* handle, bstr name) { return dlsym(handle, name); }
