# Contributing to drocc

The drocc monorepo houses infrastructure tooling for C. Contributions must adhere to the specific architectural constraints of each sub-project.

## General Guidelines
*   Keep the codebase modular. Cross-project dependencies should be minimized and strictly documented.
*   Tests must be written for new features
*   Repetitive code must be extracted out into the core library
*   Public API must be documented using doxygen comments.

## SaulGood Contributions
*   **DSL Modifications:** Any changes to the SaulGood DSL must maintain backward compatibility. Update the code generator to ensure the transpiled C code does not trigger compiler warnings.
*   **Isolation:** Modifications to `sgrun` must strictly preserve test isolation. Shared state between tests is a critical failure.

## Macromancer Contributions
*   **API Parity:** The most critical rule of Macromancer is the unified API. Any new feature added to the IDL must be implemented in both Zero-Cost mode and Dynamic mode. The transpiled output must present the exact same `#define` usage syntax to the end user.
*   **Vtable generation:** Ensure that dynamic mode vtables are generated efficiently and do not pollute the global namespace unnecessarily.

## chucci Contributions
*   **Stream-Based Architecture:** Do not construct monolithic data structures (like a single massive AST for an entire file). Data must flow between `lex`, `preprocess`, `parse`, and `codegen` as iterators/streams.
*   **Separation of Concerns:** `chucci_lex` must not resolve macros. `chucci_preprocess` must not build ASTs. Maintain the strict boundaries between the libraries.
