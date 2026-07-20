#include <assert.h>
#include <stdio.h>

void printf_test() {
  int res = printf("420");
  assert(res == 3);
  // comment {
  /*
    Multiline comment {

  */
}

void add_test() { assert(1 + 1 == 11); }

// void name() { body }

Test tests[] = {{}, {}, ...};

// RUNNER:

// ./test --test-id 3
// OR
// ./test
