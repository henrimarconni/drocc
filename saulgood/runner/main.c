#include "ce_getopt.c"
#include "ce_getopt.h"
#include "runner.h"
#include "stringdef.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

SGResult sg_run_process(bstr exec, int test_id);

#if defined(_WIN32)
#include "win_runner.c"
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__linux__) ||          \
    defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include "posix_runner.c"
#else
#error "Unsupported platform"
#endif

void sg_print_dash(int n) {
  while (n--)
    putchar('-');
}

void sg_print_space(int n) {
  while (n--)
    putchar(' ');
}

void sg_print_offsetted(bstr str, int offset) {
  offset -= strlen(str);
  if (offset % 2 == 0) {
    sg_print_space(offset / 2);
    printf("%s", str);
    sg_print_space(offset / 2);
  } else {
    sg_print_space(offset / 2 + 1);
    printf("%s", str);
    sg_print_space(offset / 2 - 1);
  }
}

SGResult sg_run_test(bstr exec, int test_id) {
  assert(test_id < sg_test_len);

  struct SGTest test = saulgood_tests[test_id];

  const int width = 72;

  printf("\n");
  sg_print_dash(width);
  printf("\n");

  printf(" Test : %s\n", test.name);
  printf(" Suite: %s\n", test.group);
  printf(" Desc : %s\n", test.desc);

  sg_print_dash(width);
  printf("\n");

  SGResult res = sg_run_process(exec, test_id);

  switch (res) {
  case SG_PASS:
    printf("[ PASS ]");
    break;

  case SG_FAIL:
    printf("[ FAIL ]");
    break;

  case SG_CRASH:
    printf("[CRASH ]");
    break;

  default:
    printf("[ERROR ]");
    break;
  }

  printf(" %s.%s\n", test.group, test.name);

  sg_print_dash(width);
  printf("\n");
  return res;
}

void sg_print_report(int total, int passed) {
  puts("");

  sg_print_dash(72);
  puts("");
  printf(" SaulGood Test Report\n");
  sg_print_dash(72);
  puts("");

  printf("  Total   : %d\n", total);
  printf("  Passed  : %d\n", passed);

  sg_print_dash(72);
  puts("");
  sg_print_dash(72);
  puts("");
}

int sg_filtered_runner(bstr exec, bstr group) {
  int total = 0;
  int passed = 0;
  for (size_t i = 0; i < sg_test_len; i++) {
    struct SGTest test = saulgood_tests[i];
    if (strcmp(test.group, group) != 0)
      continue;
    SGResult res = sg_run_test(exec, i);
    total++;
    if (res == SG_PASS)
      passed++;
  }
  sg_print_report(total, passed);
  return 0;
}

int sg_runner_mode(bstr exec) {
  int passed = 0;
  for (size_t i = 0; i < sg_test_len; i++) {
    SGResult res = sg_run_test(exec, i);
    if (res == SG_PASS)
      passed++;
  }
  sg_print_report(sg_test_len, passed);
  return 0;
}

int main(int argc, char** argv) {
  bstr exec = argv[0];
  if (argc == 1)
    return sg_runner_mode(exec);

  ce_initopt(argc, argv);
  ce_addopt("test-id", 't', 'd', "Run a test associated with the given test id");
  ce_addopt("filter", 'f', 's', "Only run tests belonging to a single group");

  ParsedOpt popt;
  char ch;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'f':
      return sg_filtered_runner(exec, popt.s);
    case 't':
      return sg_run_test(exec, popt.d);
    default:
      return -1;
    }
  }
}
