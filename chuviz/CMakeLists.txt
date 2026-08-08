
add_executable(
  chuviz
  src/main.c
  src/render.c
)

target_include_directories(
  chuviz
  PUBLIC
  src/chuviz
)

target_link_libraries(
  chuviz
  PUBLIC
  core
  chucci_lex
)
