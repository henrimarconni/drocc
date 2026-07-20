

add_library(
  core STATIC
  src/ce_getopt.c
  src/span.c
  src/vmem_arena.c
  src/utils.c
  src/parser_utils.c
)

target_include_directories(
  core
  PUBLIC
  src
)
