
include(FetchContent)
FetchContent_Declare(
  libterm
  GIT_REPOSITORY https://github.com/rizukirr/libterm
  GIT_TAG        v0.1.2
)
FetchContent_MakeAvailable(libterm)


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
  Libterm::libterm
)
