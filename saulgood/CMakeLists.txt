add_executable(
  saulgood
  src/main.c
  src/parser.c
  src/codegen.c
  src/sg_diag.c
)

target_include_directories(
  saulgood PRIVATE src
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)
