
add_library(
  chucci_sem
  src/symbol.c
  src/analyzer.c
  src/scope.c
)

target_include_directories(
  chucci_sem
  PUBLIC
  src
)

target_link_libraries(
  chucci_sem
  core
  chucci_parse
  drocc_warn
)

