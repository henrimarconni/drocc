add_executable(
  amalgamator
  src/main.c
  src/amalgamate.c
)

target_include_directories(
  amalgamator
  PRIVATE
  src
)

target_link_libraries(
  amalgamator
  PRIVATE
  core
)
