
add_library(
  chucci_ir
  src/ir.c
)

target_include_directories(
  chucci_ir
  PUBLIC
  chucci_ir/src
)

target_link_libraries(
  chucci_ir
  core
  chucci_parse
  drocc_warn
)
