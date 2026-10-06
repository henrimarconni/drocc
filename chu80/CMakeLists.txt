
add_library(
  chu80
  src/diag.c
  src/lexer.c
  src/parser.c
  src/strmap.c
)

target_link_libraries(
  chu80
  core
  drocc_warn
)

target_include_directories(
  chu80
  PUBLIC
  src
)
