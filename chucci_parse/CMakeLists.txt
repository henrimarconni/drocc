
add_library(
  chucci_parse
  STATIC
  src/parser.c
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
  core
  chucci_lex
)
