add_executable(
  saulgood
  src/main.c
  src/parser.c
  src/codegen.c
  src/sg_diag.c
)

target_include_directories(
  saulgood PRIVATE
  src
)

target_amalgamation(saulgood
  NAME runner
  INPUT "${CMAKE_SOURCE_DIR}/saulgood/runner/main.c"
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/runner.c"
  FLAGS
    -I ${CMAKE_SOURCE_DIR}/saulgood/runner/
    -I ${CMAKE_SOURCE_DIR}/core/src
)

target_amalgamation(saulgood
  NAME runner_h
  INPUT "${CMAKE_SOURCE_DIR}/saulgood/runner/runner.h"
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/runner.h"
  FLAGS
    -I ${CMAKE_SOURCE_DIR}/saulgood/runner/
    -I ${CMAKE_SOURCE_DIR}/core/src
)

target_compile_definitions(
  saulgood
  PRIVATE
  RUNNER_SOURCE_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/runner.c\"
  RUNNER_HEADER_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/runner.h\"
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)

saulgood_add_tests(
  TARGET saulgood
  FILES 
    tests/test1.sg
)
