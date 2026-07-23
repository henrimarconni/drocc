add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/runner.c
  COMMAND
    $<TARGET_FILE:amalgamator>
    ${CMAKE_SOURCE_DIR}/saulgood/runner/main.c
    -I ${CMAKE_SOURCE_DIR}/saulgood/src/
    -I ${CMAKE_SOURCE_DIR}/core/src
    -o ${CMAKE_BINARY_DIR}/runner.c
  DEPENDS
    amalgamator
    runner/main.c
)

add_custom_target(amalgamate
  DEPENDS ${CMAKE_BINARY_DIR}/runner.c
)

add_executable(
  saulgood
  src/main.c
  src/parser.c
  src/codegen.c
  src/sg_diag.c
)

target_compile_definitions(saulgood PRIVATE
    RUNNER_SOURCE_PATH=\"${CMAKE_BINARY_DIR}/runner.c\"
)

add_dependencies(saulgood amalgamate)

target_include_directories(
  saulgood PRIVATE
  src
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)
