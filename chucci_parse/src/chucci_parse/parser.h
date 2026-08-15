#ifndef PARSER_H_
#define PARSER_H_

#include "chucci_lex/token_stream.h"
#include "chucci_parse/ast.h"
#include "chucci_parse/symbol.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>

/// Top level node abstract-syntax-tree stream
typedef struct {
  /// contigous symbol storage
  infvec(SymbolData) symbols;

  ///  Mapping InternID to SymbolID
  infvec(SymbolID) ident_to_symid;

  /// Mapping InternID to SymbolID specifically for struct/enum/union without any typedef
  infvec(SymbolID) ident_to_symid_tagged;

  /// Type interner
  TypeInterner* ty_int;

  /// Consumed Token Streaming
  TokenStream ts;
  VMEMArena* arena;
  SourceManager* sman;
  StringInterner* interner;
} Parser;

/**
  Create new parser
  @param ts Consumes the provided TokenStream
*/
Parser parser_new(
    TokenStream ts,
    VMEMArena* arena,
    SourceManager* sman,
    StringInterner* interner,
    TypeInterner* ty_int);

/// Get the next TopLevelNode
TopLevelNode parser_next(Parser* p);
void parser_free(Parser* p);

#endif
