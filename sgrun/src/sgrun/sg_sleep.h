#ifndef SG_SLEEP_H_
#define SG_SLEEP_H_

#ifdef _WIN32
#include <windows.h>
static inline void sleep_ms(unsigned int ms) {
    Sleep(ms);
}
#else
#include <time.h>
static inline void sleep_ms(unsigned int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
#endif

#endif
