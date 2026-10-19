
add_executable(
  chasm
  src/main.c
  src/diag.c
)

target_link_libraries(
  chasm
  chu80
)

