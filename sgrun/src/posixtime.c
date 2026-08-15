#define _XOPEN_SOURCE 500
#include "sgrun/sg_time.h"
#include <stdint.h>
#include <time.h>

double sg_posixtime() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)(ts.tv_sec * 1000) + (double)((double)ts.tv_nsec / 1000000);
}
