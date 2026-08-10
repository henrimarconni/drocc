
add_executable(
  chuviz
  src/main.c
  src/lexer_tab.c
  src/render.c
)

target_include_directories(
  chuviz
  PUBLIC
  src
)

target_link_libraries(
  chuviz
  PUBLIC
  core
  chucci_lex
)
