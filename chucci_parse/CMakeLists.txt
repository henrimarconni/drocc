
add_library(
  chucci_parse
  STATIC
  src/parser.c
)

target_link_libraries(
  chucci_parse
  PUBLIC
  core
  chucci_lex
  chucci_preproc
  drocc_warn
)

target_include_directories(
  chucci_parse
  PUBLIC
  src
)
