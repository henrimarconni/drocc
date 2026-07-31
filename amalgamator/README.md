# Amalgamator

Amalgamator is a small command-line tool that concatenates multiple C source files and expands `#include "..."` chains to produce a single C source file. This is useful for shipping a single-file library, static analysis, or compiling for constrained toolchains.

## Why use it
- Produce a single C file from many sources and includes
- Make distribution and static analysis easier

## Usage

```sh
  amalgamator source_file.c file2.c file3.c ... -I include_dir -o amalgamated.c
```

## Build

```bash
cmake --build build --target amalgamator
