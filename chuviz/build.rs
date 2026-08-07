use std::{env, path::PathBuf};

fn main() {
    let bindings = bindgen::Builder::default()
        .headers(["core_wrap.h", "chucci_lex_wrap.h"])
        .clang_args(["-I../core/src", "-I../chucci_lex/src"])
        .generate()
        .expect("Unable to generate bindings, just fetch a joint yo!");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings.write_to_file(out_path.join("bindings.rs")).expect("You didn't teach me to write to files, yo!");
}
