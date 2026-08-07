#include "sgrun/sg_time.h"
#include <stdint.h>
#include <time.h>

double sg_posixtime() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)(ts.tv_sec * 1000) + (uint64_t)(ts.tv_nsec / 1000000);
}
