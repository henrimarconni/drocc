add_executable(
  sgrun
  src/main.c
  src/capture.c
  src/loader.c
  src/sg_fmt.c
  src/runner.c
  src/sg_scheduler.c
  src/testctx.c
)

if(WIN32)
  target_sources(sgrun PRIVATE src/windl.c src/wintime.c src/win_proc.c)
  macromancer_generate(
    TARGET sgrun
    INPUT "src/platf_loader.mm"
    FLAGS -e "SGDll=WinDll"
  )
  macromancer_generate(
    TARGET sgrun
    INPUT "src/platf_proc.mm"
    FLAGS -e "SGProc=WinProc"
  )
  macromancer_generate(
    TARGET sgrun
    INPUT "src/platf_time.mm"
    FLAGS -e "SGTime=WinTime"
  )
else()
  target_sources(sgrun PRIVATE src/posixdl.c src/posixtime.c src/posix_proc.c)
  macromancer_generate(
    TARGET sgrun
    INPUT "src/platf_loader.mm"
    FLAGS -e "SGDll=PosixDll"
  )
  macromancer_generate(
    TARGET sgrun
    INPUT "src/platf_proc.mm"
    FLAGS -e "SGProc=PosixProc"
  )
  macromancer_generate(
    TARGET sgrun
    INPUT "src/platf_time.mm"
    FLAGS -e "SGTime=PosixTime"
  )
endif()

target_include_directories(
  sgrun
  PRIVATE
  src
)

target_link_libraries(
  sgrun
  PRIVATE
  core
  drocc_warn
)

# Example test
saulgood_add_tests(
  TARGET saulgood
  FILES 
    tests/test1.test
)
