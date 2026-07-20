add_executable(
  macromancer
  src/main.c
  src/codegen.c
  src/parser.c
  src/diagnostics.c
)

target_include_directories(
  macromancer PRIVATE src
)

target_link_libraries(
  macromancer
  PRIVATE
  core
)
