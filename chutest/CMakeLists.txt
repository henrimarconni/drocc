
add_executable(
  chutest
  main.c
)

target_link_libraries(
  chutest
  PUBLIC
  core
  chucci_lex
  chucci_preproc
  chucci_diag
  chucci_parse
)
