

add_library(
  core STATIC
  src/ce_getopt.c
  src/span.c
  src/vmem_arena.c
  src/utils.c
  src/stringbuilder.c
  src/scanner.c
  src/clexer_utils.c
  src/diagnostics.c
  src/cli_diag.c
)

target_include_directories(
  core
  PUBLIC
  src
)
