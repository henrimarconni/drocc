# drocc

drocc is a monorepo containing a suite of modern, modular build tools and compiler infrastructure for C. 

## Projects

### 1.1. SaulGood
A domain-specific language (DSL) and test runner designed to eliminate the need for complex macros and runtime test registration in C. 
*   **Code Generator:** Transpiles the testing DSL directly into C code.

### 1.2. sgrun
*   **Process Isolation:** Each test is run as a different process.
*   **Parallelism:** Tests are run in parallel with a custom round-robin job scheduler
*   **Timeout:** It has a timeout to kill "vampire" tests (by default of 10 seconds)
*   **Output Capturing:** Stdout and Stderr are by default captured and only printed when a test fails (or can be printed using --show-output cli flag)

### 2. Macromancer
An Interface Definition Language (IDL) targeting C. It separates the API from the implementation, allowing users to switch implementations without any change in API
*   **Zero-Cost Abstraction Mode:** Interfaces are resolved at compile-time and transpiled into simple `#defines`.
*   **Dynamic Mode:** Utilizes underlying vtables for runtime polymorphism, transparently wrapped in `#defines`.
*   **Unified API:** The end-user API remains identical regardless of whether static or dynamic mode is used.

### 3. chucci (Work in Progress)
Aiming to be a highly modular and efficient C compiler built on a stream-based (pull-based) architecture. The compiler is segmented into independent libraries, allowing each phase to be tested and utilized in isolation:
*   `chucci_lex`
*   `chucci_preprocess`
*   `chucci_parse`
*   `chucci_codegen`
*   `chucci_asm`

### 4. Amalgamator
A small command-line tool that concatenates multiple C source files and expands #include "..." chains to produce a single C source file. This is useful for shipping a single-file library, static analysis, or compiling for constrained toolchains.
