
add_library(
  volecore
  STATIC
  src/diag.c
)

target_include_directories(
  volecore
  PUBLIC
  ../include
)

target_link_libraries(
  volecore
  core
  drocc_warn
)
