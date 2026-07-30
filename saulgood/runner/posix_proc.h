#ifndef SG_POSIX_PROC_H
#define SG_POSIX_PROC_H

#include "core/stringdef.h"
#include "process.h"

SGProcess* posix_spawn_proc(bstr exe_path, bstr const* argv, unsigned flags);
int posix_wait_proc(SGProcess*);
int posix_trywait_proc(SGProcess*);
int posix_kill_proc(SGProcess*);
void posix_free_proc(SGProcess*);
ostr posix_proc_take_stdout(SGProcess*);
ostr posix_proc_take_stderr(SGProcess*);
SGProcessStatus posix_proc_status(SGProcess* proc);

#endif
