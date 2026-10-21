
add_library(
  voparser
  STATIC
  src/parser.c
  src/scope.c
  src/type.c
  src/expr.c
  src/stmt.c
)

target_link_libraries(
  voparser
  core
  volexer
  drocc_warn
)
