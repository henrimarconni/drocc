add_executable(
  saulgood
  src/main.c
)

target_include_directories(
  saulgood PRIVATE src
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)
