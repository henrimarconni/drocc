
add_library(
  chucci_parse
  STATIC
  src/parser.c
  src/typeinterner.c
  src/decl_spec.c
  src/declarator.c
  src/type.c
)

target_link_libraries(
  chucci_parse
  PUBLIC
  core
  chucci_lex
  chucci_preproc
  chucci_diag
  drocc_warn
)

target_include_directories(
  chucci_parse
  PUBLIC
  src
)
