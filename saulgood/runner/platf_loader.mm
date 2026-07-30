
$interface SGDll as Static {
# void* sgdl_load(char*);
  sgdl_load
  
# void  sgdl_unload(void*);
  sgdl_unload

# void* sgdl_get(void*, bstr);
  sgdl_get
}

$impl WinDll as SGDll {
  $header     = "windl.h"
  sgdl_load   = windl_load
  sgdl_unload = windl_unload
  sgdl_get    = windl_get
}

$impl PosixDll as SGDll {
  $header     = "dlfcn.h"
  sgdl_load   = dlopen
  sgdl_unload = dlclose
  sgdl_get    = dlsym
}

# $export SGDll as PosixDll
