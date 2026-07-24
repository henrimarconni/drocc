#ifndef SG_RUNNER_H
#define SG_RUNNER_H

struct SGTest {
  const char* name;
  const char* group;
  const char* desc;
  void (*fn)();
};

extern const int sg_test_len;
extern struct SGTest saulgood_tests[];

#endif
