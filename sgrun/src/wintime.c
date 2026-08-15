#include "sgrun/sg_time.h"
#include "windows.h"
#include <stdint.h>

double sg_wintime() {
  LARGE_INTEGER frequency;
  LARGE_INTEGER current_time;

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&current_time);

  return (double)((current_time.QuadPart * 1000) / frequency.QuadPart);
}
