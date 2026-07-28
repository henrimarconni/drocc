
# SaulGood 
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

target_compile_definitions(
  saulgood
  PRIVATE
  SG_RUNNER_API=\"${CMAKE_CURRENT_SOURCE_DIR}/runner/sg_api.h\"
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)

# SaulGood Runner
add_executable(
  sgrun
  runner/main.c
  runner/capture.c
  runner/loader.c
)

target_link_libraries(
  sgrun
  PRIVATE
  core
)

# Example test
saulgood_add_tests(
  TARGET saulgood
  FILES 
    tests/test1.sg
)
