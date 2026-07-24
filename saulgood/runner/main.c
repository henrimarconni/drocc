#include "ce_getopt.c"
#include "ce_getopt.h"
#include "runner.h"
#include "stringdef.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int sg_run_test(int test_id) {
  assert(test_id < sg_test_len);
  saulgood_tests[test_id].fn();
  return 0;
}

void sg_wait_pid(int pid, int i) {
  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
    exit(-1);
  }
  struct SGTest test = saulgood_tests[i];
  if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    printf("[PASS] %s.%s\n", test.group, test.name);
  else
    printf("[FAIL] %s.%s : %s\n", test.group, test.name, test.desc);
}
int sg_filtered_runner(bstr group) {
  for (size_t i = 0; i < sg_test_len; i++) {
    if (strcmp(saulgood_tests[i].group, group) != 0)
      continue;
    int pid = fork();
    if (pid < 0) {
      perror("fork");
      return -1;
    }
    if (pid == 0) {
      saulgood_tests[i].fn();
      exit(0);
    }

    sg_wait_pid(pid, i);
  }
  return 0;
}

int sg_runner_mode() {
  for (size_t i = 0; i < sg_test_len; i++) {
    int pid = fork();
    if (pid < 0) {
      perror("fork");
      return -1;
    }
    if (pid == 0) {
      saulgood_tests[i].fn();
      exit(0);
    }
    sg_wait_pid(pid, i);
  }
  return 0;
}

int main(int argc, char** argv) {
  if (argc == 1)
    return sg_runner_mode();

  ce_initopt(argc, argv);
  ce_addopt("test-id", 't', 'd', "Run a test associated with the given test id");
  ce_addopt("filter", 'f', 's', "Only run tests belonging to a single group");

  ParsedOpt popt;
  char ch;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'f':
      return sg_filtered_runner(popt.s);
    case 't':
      return sg_run_test(popt.d);
    default:
      return -1;
    }
  }
}
