#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use core::slice;
use std::{ffi::CString};

use crate::c::*;

#[allow(warnings)]
pub mod c {
    include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
}

impl Token {
    pub fn print(&self, sman: *mut SourceManager) {
        unsafe {
            let info = sman_info(sman, self.span);
            let slice = slice::from_raw_parts(info.sv.str_ as *const u8, info.sv.len as usize);
            let sstr = str::from_utf8_unchecked(slice);
            print!("{}", sstr);
        }
    }
}

fn main() {
    unsafe {
        let cstr = CString::new("int main() { return 0; }").unwrap();
        let cstr_len = cstr.count_bytes();
        let name = CString::new("scratch").unwrap();
        let mut sman = c::sman_new();
        let sman_ptr = &mut sman as *mut SourceManager;
        let arena = vmarena_new(128*1024);
        let interner = interner_new(arena);
        let srcid = sman_str(sman_ptr, name.into_raw(), cstr.into_raw(), cstr_len);

        let mut lexer_stream = lexer_new(sman_ptr, srcid, interner);
        let ts = &mut lexer_stream as *mut TokenStream;
        let mut token = ts_next(ts);
        while token.kind != TokenKind_TOK_EOF {
            token.print(sman_ptr);
            print!(" ");
            token = ts_next(ts);
        }
        println!("");
        
        sman_free(sman_ptr);
        vmarena_free(arena);
    }
}
