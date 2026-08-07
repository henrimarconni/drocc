#ifndef SG_SLEEP_H_
#define SG_SLEEP_H_

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000) // usleep uses microseconds
#endif

#endif
