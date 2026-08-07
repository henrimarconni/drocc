#ifndef SG_RUNNER_API_H
#define SG_RUNNER_API_H

#include <stdio.h>

struct SGTest {
  const char* name;
  const char* group;
  const char* desc;
  void (*fn)();
};

typedef struct {
  FILE* stream;
  FILE* tmp;
  int saved_fd;
} SGCapture;

typedef struct {
  void (*capture_begin)(SGCapture* cap, FILE* stream);
  char* (*capture_end)(SGCapture* cap);
  void (*capture_discard)(SGCapture* cap);
} SGRuntime;

#ifndef SG_RUNNER_DEV
SGRuntime saulg;
#endif

#endif
