
add_executable(
  chucci_preproc
  src/preproc.c
)

target_link_libraries(
  chucci_preproc
  PUBLIC
  chucci_lex
  core
)
