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
  SG_RUNNER_API=\"${CMAKE_SOURCE_DIR}/sgrun/src/sgrun/sg_api.h\"
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)

