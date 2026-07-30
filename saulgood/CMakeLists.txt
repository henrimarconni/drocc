
# SaulGood 
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
  SG_RUNNER_API=\"${CMAKE_CURRENT_SOURCE_DIR}/runner/sg_api.h\"
)

target_link_libraries(
  saulgood
  PRIVATE
  core
)

# SaulGood Runner
add_executable(
  sgrun
  runner/main.c
  runner/capture.c
  runner/loader.c
  runner/sg_fmt.c
  runner/runner.c
  runner/sg_scheduler.c
)

if(WIN32)
  target_sources(sgrun PRIVATE runner/windl.c runner/win_proc.c)
  macromancer_generate(
    TARGET sgrun
    INPUT "runner/platf_loader.mm"
    FLAGS -e "SGDll=WinDll"
  )
  macromancer_generate(
    TARGET sgrun
    INPUT "runner/platf_proc.mm"
    FLAGS -e "SGProc=WinProc"
  )
else()
  target_sources(sgrun PRIVATE runner/posix_proc.c)
  macromancer_generate(
    TARGET sgrun
    INPUT "runner/platf_loader.mm"
    FLAGS -e "SGDll=PosixDll"
  )
  macromancer_generate(
    TARGET sgrun
    INPUT "runner/platf_proc.mm"
    FLAGS -e "SGProc=PosixProc"
  )
endif()

target_include_directories(
  sgrun
  PRIVATE
  runner
)

target_link_libraries(
  sgrun
  PRIVATE
  core
)

# Example test
saulgood_add_tests(
  TARGET saulgood
  FILES 
    tests/test1.sg
)
