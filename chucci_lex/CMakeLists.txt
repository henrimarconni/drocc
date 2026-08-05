
add_library(
  chucci_lex
  STATIC
  src/lexer.c
  src/token.c
  src/cc_diag.c
  src/token_stream.c
)

target_include_directories(
  chucci_lex
  PUBLIC
  src
)

target_link_libraries(
  chucci_lex
  PUBLIC
  core
)

