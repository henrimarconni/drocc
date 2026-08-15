add_executable(
  saulgood
  src/main.c
  src/parser.c
  src/codegen.c
  src/sg_diag.c
)

target_include_directories(
  saulgood
  PRIVATE
  src
)

find_package(Python3 REQUIRED)

set(SG_API_PATH "${CMAKE_SOURCE_DIR}/sgrun/src/sgrun/sg_api.h")
set(SG_API_EMBEDDED "${CMAKE_CURRENT_BINARY_DIR}/sg_api_data.inc")

add_custom_command(
  OUTPUT ${SG_API_EMBEDDED}
  COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/scripts/file2c.py 
          ${SG_API_PATH} 
          ${SG_API_EMBEDDED}
  DEPENDS ${SG_API_PATH} ${CMAKE_SOURCE_DIR}/scripts/file2c.py
)

target_sources(saulgood PRIVATE src/codegen.c ${SG_API_EMBEDDED})

target_include_directories(saulgood PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(
  saulgood
  PRIVATE
  SG_RUNNER_API=\"${CMAKE_SOURCE_DIR}/sgrun/src/sgrun/sg_api.h\"
)

target_link_libraries(
  saulgood
  PRIVATE
  core
  drocc_warn
)

