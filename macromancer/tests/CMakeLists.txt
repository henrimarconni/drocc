add_executable(
  macromancer_test
  main.c
)

macromancer_generate(
  TARGET macromancer_test
  INPUT "animals.mm"
)
