#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

/**
    AI GENERATED!! I am too bored to write wrappers myself.
*/
use std::ffi::{CStr, CString};
use std::os::raw::c_char;

// Adjust this path if your bindings.rs is generated somewhere else
#[allow(warnings)]
pub mod c {
    include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
}

// ==========================================
// 1. MEMORY ARENA
// ==========================================
pub struct Arena {
    arena: *mut c::VMEMArena,
}

impl Arena {
    pub fn new(size: usize) -> Self {
        unsafe {
            Self {
                arena: c::vmarena_new(size),
            }
        }
    }

    pub(crate) fn as_raw(&self) -> *mut c::VMEMArena {
        self.arena
    }
}

impl Drop for Arena {
    fn drop(&mut self) {
        unsafe { c::vmarena_free(self.arena) }
    }
}

// ==========================================
// 2. SOURCE MANAGER
// ==========================================
pub struct SourceManager {
    // Note: sman_new returns by value, so we hold the struct natively
    sman: c::SourceManager,
}

impl SourceManager {
    pub fn new() -> Self {
        unsafe {
            Self {
                sman: c::sman_new(),
            }
        }
    }

    pub(crate) fn as_raw_mut(&mut self) -> *mut c::SourceManager {
        &mut self.sman as *mut c::SourceManager
    }

    /// Opens a file and allocates its contents into the Arena.
    pub fn open(&mut self, filename: &str, arena: &Arena) -> Result<u16, String> {
        let c_filename = CString::new(filename).unwrap();
        unsafe {
            let srcid = c::sman_open(
                self.as_raw_mut(),
                c_filename.as_ptr() as *mut c_char,
                arena.as_raw(),
            );

            if srcid as u32 == c::INVALID_SRC_ID {
                Err(format!("Failed to open file: {}", filename))
            } else {
                Ok(srcid)
            }
        }
    }
}

impl Drop for SourceManager {
    fn drop(&mut self) {
        unsafe { c::sman_free(self.as_raw_mut()) }
    }
}

// ==========================================
// 3. STRING INTERNER
// ==========================================
pub struct StringInterner {
    interner: *mut c::StringInterner,
}

impl StringInterner {
    pub fn new(arena: &Arena) -> Self {
        unsafe {
            Self {
                interner: c::interner_new(arena.as_raw()),
            }
        }
    }

    pub(crate) fn as_raw(&self) -> *mut c::StringInterner {
        self.interner
    }
}

impl Drop for StringInterner {
    fn drop(&mut self) {
        unsafe { c::interner_free(self.interner) }
    }
}

// ==========================================
// 4. TOKENS
// ==========================================
pub struct Token {
    token: c::Token,
}

impl Token {
    pub fn kind(&self) -> u32 {
        self.token.kind
    }

    pub fn ident_id(&self) -> u32 {
        self.token.ident
    }

    /// Safely maps the C enum to a readable string using the global tok_to_str array
    pub fn kind_str(&self) -> &'static str {
        unsafe {
            let kind_idx = self.token.kind as usize;

            // Bounds check to prevent segfaults
            if kind_idx >= c::TokenKind___token_kind_count as usize {
                return "UNKNOWN_OOB";
            }

            let ptr = c::tok_to_str[kind_idx];
            if ptr.is_null() {
                return "UNKNOWN_NULL";
            }

            CStr::from_ptr(ptr).to_str().unwrap_or("INVALID_UTF8")
        }
    }
}

// ==========================================
// 5. TOKEN STREAM
// ==========================================
pub struct TokenStream {
    ts: c::TokenStream,
    is_eof: bool, // Helps our Iterator know when to stop
}

impl TokenStream {
    // Correctly mapped to lexer_new
    pub fn from_lexer(sman: &mut SourceManager, srcid: u16, interner: &StringInterner) -> Self {
        unsafe {
            Self {
                ts: c::lexer_new(sman.as_raw_mut(), srcid, interner.as_raw()),
                is_eof: false,
            }
        }
    }

    // You can easily add the others later when you need them!
    // pub fn from_token(token: &Token) -> Self { ... }
    // pub fn from_vec(vec: ...) -> Self { ... }

    pub(crate) fn as_raw_mut(&mut self) -> *mut c::TokenStream {
        &mut self.ts as *mut c::TokenStream
    }

    pub fn peek(&mut self) -> Token {
        unsafe {
            Token {
                token: c::ts_peek(self.as_raw_mut()),
            }
        }
    }
}

impl Drop for TokenStream {
    fn drop(&mut self) {
        unsafe { c::ts_free(self.as_raw_mut()) }
    }
}

impl Iterator for TokenStream {
    type Item = Token;

    fn next(&mut self) -> Option<Self::Item> {
        if self.is_eof {
            return None;
        }

        unsafe {
            let next_tok = c::ts_next(self.as_raw_mut());

            if next_tok.kind == c::TokenKind_TOK_EOF {
                self.is_eof = true;
                Some(Token { token: next_tok })
            } else {
                Some(Token { token: next_tok })
            }
        }
    }
}
