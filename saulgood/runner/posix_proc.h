#ifndef SG_POSIX_PROC_H
#define SG_POSIX_PROC_H

#include "core/stringbuilder.h"
#include "process.h"

SGProcess* posix_spawn_proc(bstr exe_path, bstr const* argv, unsigned flags);
int posix_wait_proc(SGProcess*);
int posix_trywait_proc(SGProcess*);
int posix_kill_proc(SGProcess*);
void posix_free_proc(SGProcess*);
bool posix_proc_pump_stdout(SGProcess* proc, StringBuilder* b);
bool posix_proc_pump_stderr(SGProcess* proc, StringBuilder* b);
SGProcessStatus posix_proc_status(SGProcess* proc);

#endif
