use chuviz::ffi::{self, Arena, SourceManager, StringInterner, TokenStream};

fn main() {
    let arena = Arena::new(128 * 1024);
    let mut interner = StringInterner::new(&arena);
    let mut sman = SourceManager::new();
    let srcid = sman.open(filename, arena)
    let mut ts = TokenStream::from_lexer(&mut sman, srcid, &interner);
    print!("\n");
}
