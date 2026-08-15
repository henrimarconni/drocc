#ifndef SIGNALS_H_
#define SIGNALS_H_

void register_crash_handlers(void (*handler)(int sig));

#endif
