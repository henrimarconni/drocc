cmake_minimum_required(VERSION 3.14)
project(drocc)

enable_testing()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/InternalModules.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/InternalModules.cmake")
  message(STATUS "Loaded modules natively from drocc directories.")
else()
  find_package(Saulgood REQUIRED)
  message(STATUS "Loaded modules from system installation.")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)

add_subdirectory(core)
add_subdirectory(amalgamator)
add_subdirectory(macromancer)
add_subdirectory(saulgood)
add_subdirectory(sgrun)
add_subdirectory(chucci_lex)

# add_subdirectory(core/tests)
# add_subdirectory(amalgamator/tests)
add_subdirectory(macromancer/tests macromancer_tests)
# add_subdirectory(saulgood/tests)
add_subdirectory(chucci_lex/tests chucci_lex_tests)
