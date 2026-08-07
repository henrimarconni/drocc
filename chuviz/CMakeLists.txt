include(FetchContent)

FetchContent_Declare(
  Corrosion
  GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
  GIT_TAG v0.6.1
)
FetchContent_MakeAvailable(Corrosion)
corrosion_import_crate(MANIFEST_PATH Cargo.toml)

corrosion_link_libraries(
  chuviz
  chucci_lex
  core
)

corrosion_add_target_local_rustflags(
  chuviz 
  "-Clink-arg=-B/usr/bin"
  "-Clink-arg=-fuse-ld=mold"
)
