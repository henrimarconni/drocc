
$interface SGDll as Static {
# void* sgdl_load(char*);
  sgdl_load
  
# void  sgdl_unload(void*);
  sgdl_unload

# void* sgdl_get(void*, bstr);
  sgdl_get
}

$impl WinDll as SGDll {
  $header     = "sgrun/windl.h"
  sgdl_load   = windl_load
  sgdl_unload = windl_unload
  sgdl_get    = windl_get
}

$impl PosixDll as SGDll {
  $header     = "sgrun/posixdl.h"
  sgdl_load   = posixdl_load
  sgdl_unload = posixdl_unload
  sgdl_get    = posixdl_get
}
