function(target_amalgamation TARGET NAME INPUT OUTPUT)
  add_custom_command(
    OUTPUT ${OUTPUT}
    COMMAND
      $<TARGET_FILE:amalgamator>
      ${INPUT}
      ${ARGN}
      -o ${OUTPUT}
    DEPENDS
      amalgamator
      ${INPUT}
  )

  add_custom_target(${NAME}_amalgamation DEPENDS ${OUTPUT})
  add_dependencies(${TARGET} ${NAME}_amalgamation)
endfunction()

add_executable(
  saulgood
  src/main.c
  src/parser.c
  src/codegen.c
  src/sg_diag.c
)

target_amalgamation(
  saulgood
  runner
  ${CMAKE_SOURCE_DIR}/saulgood/runner/main.c
  ${CMAKE_BINARY_DIR}/runner.c
  -I ${CMAKE_SOURCE_DIR}/saulgood/runner/
  -I ${CMAKE_SOURCE_DIR}/core/src
)

target_amalgamation(
  saulgood
  runner_h
  ${CMAKE_SOURCE_DIR}/saulgood/runner/runner.h
  ${CMAKE_BINARY_DIR}/runner.h
  -I ${CMAKE_SOURCE_DIR}/saulgood/runner/
  -I ${CMAKE_SOURCE_DIR}/core/src
)

target_compile_definitions(
  saulgood
  PRIVATE
  RUNNER_SOURCE_PATH=\"${CMAKE_BINARY_DIR}/runner.c\"
  RUNNER_HEADER_PATH=\"${CMAKE_BINARY_DIR}/runner.h\"
)

target_include_directories(
  saulgood PRIVATE
  src
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)
