#ifndef SG_RUNNER_H
#define SG_RUNNER_H

typedef enum { SG_PASS, SG_FAIL, SG_CRASH, SG_ERROR } SGResult;

struct SGTest {
  const char* name;
  const char* group;
  const char* desc;
  void (*fn)();
};

extern const int sg_test_len;
extern struct SGTest saulgood_tests[];

#endif
