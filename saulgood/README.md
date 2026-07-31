# SaulGood

SaulGood is a C testing framework that replaces macro-heavy, runtime-registered test suites with a clean Domain-Specific Language (DSL).

## Rationale
Standard C testing libraries rely on convoluted preprocessor macros and manual runtime registration to discover and execute tests. This leads to obscure compiler errors, bloated test binaries, and fragile global state. SaulGood solves this by moving test definition into a DSL that is pre-processed before the C compiler sees it.

## Architecture
1.  **Transpilation:** The SaulGood transpiler reads the test DSL and generates clean, standard C code.
2.  **Compilation:** The generated C code is compiled into a standalone shared library (`.so`).
3.  **Execution:** The SaulGood test runner (`sgrun`) loads the `.so` dynamically. It manages execution, ensuring every test runs in a completely isolated environment to prevent state leakage between test cases.

## Example
Here's a simple example

```c
$c {
  #include<stdio.h>
}

$test "Test printf return value" : stdio(printf_return) {
  // You can use any standard C code here
  // stdout and stderr are only printed if the test fails
  // You can capture stdout and stderr by dup2, or using the simple SGCapture api proveded by saulgood
  int res = printf("69420");
  assert(res == 5);
}

$test "Test if 1 + 1 == 11" : math(unity_in_diversity) {
  assert(1 + 1 == 11);
}

```

Generate the code:

```sh
  saulgood printf.test -o printf_test.c
```

Compile generated code to a dynamic library:

```sh
  gcc -shared -fPIC printf_test.c -o printf_test.so
```

Run the tests:
```sh
  sgrun -j 2 ./printf_test.so
```

This will run atmost 2 tests in parallel and print the report.
