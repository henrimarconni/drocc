
$interface SGProc as Static {
# return 0 on success
# SGProcess* sg_spawn_proc(bstr exe_path, bstr const* argv, unsigned flags);
  sg_spawn_proc

# return 0 on success
# int sg_wait_proc(SGProcess*);
  sg_wait_proc

# return 0 if completed
# int sg_trywait_proc(SGProcess*);
  sg_trywait_proc
  
# int sg_kill_proc(SGProcess*);
  sg_kill_proc
  
# int sg_free_proc(SGProcess*);
  sg_free_proc

# SGProcessStatus sg_proc_status(SGProcess*);
  sg_proc_status

  # ostr posix_proc_take_stdout(SGProcess*);
  sg_proc_take_stdout

  # ostr posix_proc_take_stderr(SGProcess*);
  sg_proc_take_stderr
}

$impl WinProc as SGProc {
  $header         = "win_proc.h"
  sg_spawn_proc   = win_spawn_proc  
  sg_wait_proc    = win_wait_proc  
  sg_trywait_proc = win_trywait_proc  
  sg_kill_proc    = win_kill_proc  
  sg_free_proc    = win_free_proc  
  sg_proc_status  = win_proc_status
  sg_proc_take_stdout = win_proc_take_stdout
  sg_proc_take_stderr = win_proc_take_stderr
}

$impl PosixProc as SGProc {
  $header         = "posix_proc.h"
  sg_spawn_proc   = posix_spawn_proc
  sg_wait_proc    = posix_wait_proc
  sg_trywait_proc = posix_trywait_proc
  sg_kill_proc    = posix_kill_proc
  sg_free_proc    = posix_free_proc
  sg_proc_status  = posix_proc_status
  sg_proc_take_stdout = posix_proc_take_stdout
  sg_proc_take_stderr = posix_proc_take_stderr
}

