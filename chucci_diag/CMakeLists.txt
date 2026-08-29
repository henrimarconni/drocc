
add_library(
  chucci_diag
  STATIC
  src/cc_diag.c
)

target_include_directories(
  chucci_diag
  PUBLIC
  src
)

target_link_libraries(
  chucci_diag
  PUBLIC
  core
)
