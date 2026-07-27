# TODO

This file tracks long-term work across the Drocc Compiler Collection.

## High Priority

### Core

* [ ] Finish unit tests for all shared utilities.
* [ ] Review public API consistency.
* [ ] Document code using Doxygen format

### SaulGood

* [ ] Add examples.
* [ ] Add parallel test running
* [ ] Explore the idea of tests getting compiled to a library and being run using saulgood runner instead of this amalgamated setup

### Amalgamator

* [ ] Impove include cache implementation.
* [ ] Skip preprocessor directives inside strings. (this should be fixed after chucci_front is implemented)
* [ ] Improve path resolution.
* [ ] Add tests.

### Macromancer

* [ ] Expand documentation.
* [ ] Add examples.
* [ ] Add tests.

### Chucci Compiler

* [ ] Fill Todo

### Drocc

* [ ] Add installation support.
* [ ] Add CI/CD
* [ ] Homepage
* [ ] Website.
* [ ] User guide.
* [ ] Contribution guide.
* [ ] Release notes generator.
* [ ] Project templates.

---

# Refactoring

## Naming

* [ ] Namespace all public symbols.
* [ ] Prefix public types with project code.

  * `SGCodegen`
  * `MMCodegen`
  * `AMParser`
  * ...
* [ ] Review enums.
* [ ] Review macros.
* [ ] Review public functions.

---

# Documentation

* [ ] Finish Doxygen comments.
* [ ] Add project overviews.
* [ ] Document public APIs only.
* [ ] Publish documentation through CI.

---

# Build System

* [ ] Finish install rules.
* [ ] Export CMake packages.
* [ ] Verify standalone installation.

---

# CI

* [ ] Build every project.
* [ ] Run all tests.
* [ ] Generate documentation.
* [ ] Check formatting.
* [ ] Upload documentation.

---

# Nice to Have

* [ ] Benchmarks.
* [ ] Fuzz testing.
* [ ] Coverage reports.
* [ ] Static analysis.
* [ ] Release automation.
* [ ] Package generation.

---

