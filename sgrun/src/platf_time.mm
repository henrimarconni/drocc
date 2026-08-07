
$interface SGTime as Static {
  # double sgtime();
  sgtime
}

$impl WinTime as SGTime {
  $header = "sgrun/sg_time.h"
  sgtime = sg_wintime
}

$impl PosixTime as SGTime {
  $header = "sgrun/sg_time.h"
  sgtime = sg_posixtime
}
