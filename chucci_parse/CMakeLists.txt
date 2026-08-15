
add_library(
  chucci_parse
  STATIC
  src/parser.c
  src/decl_spec.c
  src/declarator.c
  src/type.c
  src/typeinterner.c
)

target_include_directories(
  chucci_parse
  PUBLIC
  src
)

target_link_libraries(
  chucci_parse
  PUBLIC
  chucci_lex
  drocc_warn
)
