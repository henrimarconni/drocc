
add_library(
  volexer
  STATIC
  src/lexer.c
  src/token.c
  src/token_stream.c
)

target_include_directories(
  volexer
  PUBLIC
  ../include
)

target_link_libraries(
  volexer
  PUBLIC
  core
  volecore
  drocc_warn
)

