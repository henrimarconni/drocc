#ifndef SG_RUNNER_H
#define SG_RUNNER_H

#include "core/stringdef.h"
#include <stdio.h>

typedef enum { SG_PASS, SG_FAIL, SG_CRASH, SG_ERROR } SGResult;

struct SGTest {
  const char* name;
  const char* group;
  const char* desc;
  void (*fn)();
};

typedef struct {
    FILE *stream;
    FILE *tmp;
    int saved_fd;
} SGCapture;

void capture_begin(SGCapture *cap, FILE *stream);
ostr capture_end(SGCapture *cap);

extern const int sg_test_len;
extern struct SGTest saulgood_tests[];

#endif
