#ifndef SG_WIN_PROC_H
#define SG_WIN_PROC_H

#include "core/stringdef.h"
#include "process.h"

SGProcess* win_spawn_proc(bstr exe_path, bstr const* argv);
int  win_wait_proc(SGProcess*);
int  win_trywait_proc(SGProcess*);
int  win_kill_proc(SGProcess*);
void win_free_proc(SGProcess*);
ostr win_proc_take_stdout(SGProcess*);
ostr win_proc_take_stderr(SGProcess*);


#endif
