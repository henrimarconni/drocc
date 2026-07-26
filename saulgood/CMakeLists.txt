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

include(CMakeParseArguments)

function(saulgood_add_tests)
  set(options)
  set(oneValueArgs TARGET NAME OUTPUT)
  set(multiValueArgs FILES LIBRARIES)

  cmake_parse_arguments(SG
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )

  if(NOT SG_FILES)
    message(FATAL_ERROR "saulgood_add_tests: FILES is required.")
  endif()

  if(NOT SG_NAME)
    if(SG_TARGET)
      set(SG_NAME "${SG_TARGET}_tests")
    else()
      set(SG_NAME "saulgood_tests")
    endif()
  endif()

  if(NOT SG_OUTPUT)
    set(SG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SG_NAME}.c")
  endif()

  add_custom_command(
    OUTPUT ${SG_OUTPUT}
    COMMAND
      $<TARGET_FILE:saulgood>
      ${SG_FILES}
      -o ${SG_OUTPUT}
    DEPENDS
      saulgood
      ${SG_FILES}
    WORKING_DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_executable(${SG_NAME} ${SG_OUTPUT})

  add_dependencies(${SG_NAME} saulgood)

  if(SG_LIBRARIES)
    target_link_libraries(${SG_NAME}
      PRIVATE
      ${SG_LIBRARIES}
    )
  endif()

  if(SG_TARGET)
    get_target_property(TARGET_TYPE ${SG_TARGET} TYPE)

    if(TARGET_TYPE STREQUAL "STATIC_LIBRARY"
      OR TARGET_TYPE STREQUAL "SHARED_LIBRARY"
      OR TARGET_TYPE STREQUAL "OBJECT_LIBRARY")

      target_link_libraries(${SG_NAME}
        PRIVATE
        ${SG_TARGET}
      )

    elseif(TARGET_TYPE STREQUAL "EXECUTABLE")

      target_compile_definitions(${SG_NAME}
        PRIVATE
        SG_TARGET_EXECUTABLE="$<TARGET_FILE:${SG_TARGET}>"
      )

    else()
      message(FATAL_ERROR
        "saulgood_add_tests: Unsupported target type '${TARGET_TYPE}'."
      )
    endif()
  endif()

  add_test(
    NAME ${SG_NAME}
    COMMAND $<TARGET_FILE:${SG_NAME}>
  )
endfunction()

saulgood_add_tests(
  TARGET saulgood
  FILES tests/test1.sg
)
