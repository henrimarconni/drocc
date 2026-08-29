
add_library(
  chucci_preproc
  src/preproc.c
  src/include.c
)

target_include_directories(
  chucci_preproc
  PUBLIC
  src
)

target_link_libraries(
  chucci_preproc
  PUBLIC
  chucci_lex
  chucci_diag
  drocc_warn
  core
)
