#ifndef PARSER_H_
#define PARSER_H_

#include "chucci_lex/token_stream.h"
#include "chucci_parse/symbol.h"
#include "chucci_parse/type.h"
#include "chucci_parse/typeinterner.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <assert.h>
#include <stdint.h>

/// Declaration of variable/function
typedef struct {
  SymbolID symid;
  Type type;
  /// Can only be SC_STATIC/SC_EXTERN for functions
  StorageClass sc;
} VarFnDecl;

/// Type of Top Level Nodes
typedef enum {
  /// type var = ...;
  TLN_VAR_DEF,
  /// type fn(..) {...}
  TLN_FUNC_DEF,

  /// type fn(...); or type var;
  TLN_VARFN_DECL,

  // TODO:
  // TLN_TYPE_DECL => struct/enum/union + typedefs
} TLNASTKind;

typedef struct {
  TLNASTKind kind;
  union {
    VarFnDecl varfn_decl;
  };
} TopLevelNode;

/// Top level node abstract-syntax-tree stream
typedef struct {
  /// contigous symbol storage
  SymbolStorage symbols;

  ///  Mapping InternID to SymbolID
  vec(SymbolID) symbolids;

  /// Mapping InternID to SymbolID specifically for struct/enum/union without any typedef
  vec(SymbolID) tagged_symbolids;

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

#endif
