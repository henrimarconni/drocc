# drocc

drocc is a modular compiler and tooling collection for C. It brings together compiler infrastructure, code generation, test infrastructure, and small build utilities in one repository, with a strong emphasis on modularity, testability, and clean boundaries between components.

## Why drocc?

If you are building or experimenting with C compiler infrastructure, you want pieces that are:
- small and testable
- easy to reason about
- reusable across compiler phases
- built around plain C and CMake instead of heavyweight frameworks

drocc is built for that. It includes:
- Vole, a modular compiler stack for lexing, diagnostics, token streams, and compiler infrastructure
- Macromancer, an interface definition language and code generator for C
- Amalgamator, for collapsing many source files into a single distributable C file
- sgrun, a parallel, process-isolated test runner for C projects
- SaulGood, a tiny DSL for reducing boilerplate and macro-heavy test registration

## Project overview

### Vole
Vole, formerly chucci, is a modular C Compiler. It is organized into smaller, focused libraries under the vole/ directory.

The current layout includes:
- vole/lexer
- vole/volecore
- vole/chuviz
- vole/include

The philosophy is simple: compiler phases should be modular, testable, and composable. For example, the lexer sits in its own library, with its own source files and tests, rather than being buried in a monolithic compiler.

### Macromancer
Macromancer is an interface definition language for C that separates API from implementation. It generates plain C code and lets you switch between implementations without changing the calling API.

It supports:
- static interfaces with zero runtime overhead
- dynamic interfaces backed by vtables
- generated code that stays ordinary C

### Amalgamator
Amalgamator combines multiple C source files and expands their include chains to create a single C source file. This is useful for:
- shipping self-contained libraries
- static analysis
- easier distribution and review

### sgrun
sgrun is a test runner designed for C projects. It focuses on:
- process isolation for each test
- parallel execution with scheduling
- timeouts for hanging or runaway tests
- capturing output and showing it only on failure

### SaulGood
SaulGood is a lightweight DSL for removing repetitive boilerplate and macro-heavy registration patterns in C-based tests and tooling.

## Repository structure

```text
drocc/
├── CMakeLists.txt
├── InternalModules.cmake
├── LICENSE.txt
├── README.md
├── CONTRIBUTING.md
├── TODO.md
├── amalgamator/
├── chasm/
├── chu80/
├── core/
├── macromancer/
├── saulgood/
├── scripts/
├── sgrun/
├── vole/
│   ├── chuviz/
│   ├── include/
│   ├── lexer/
│   ├── volecore/
│   └── CMakeLists.txt
└── ...
