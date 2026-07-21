cmake_minimum_required(VERSION 3.14)
project(drift)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)

add_subdirectory(core)
add_subdirectory(amalgamator)
add_subdirectory(saulgood)
add_subdirectory(macromancer)
