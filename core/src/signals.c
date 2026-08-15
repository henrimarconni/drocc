#include "core/signals.h"
#include <signal.h>

void register_crash_handlers(void (*handler)(int sig)) {
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);
  signal(SIGFPE, handler);
  signal(SIGILL, handler);

#if defined(__unix__) || defined(__APPLE__)
  signal(SIGBUS, handler);
  signal(SIGTRAP, handler);
#endif
}
