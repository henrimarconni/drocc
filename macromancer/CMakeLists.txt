add_executable(
  macromancer
  src/main.c
  src/mm_diag.c
  src/mm_arg.c
  src/mmtok.c
  src/parser.c
  src/codegen.c
)

target_include_directories(
  macromancer PRIVATE src
)

target_link_libraries(
  macromancer
  PRIVATE
  core
)

macromancer_generate(
  TARGET macromancer
  INPUT "tests/animals.mm"
)
