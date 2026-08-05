#ifndef MMTOK_H_
#define MMTOK_H_

#include "core/srcman.h"
#include "core/stringdef.h"
#include "macromancer/parser.h"

typedef enum {
  /// =
  MMT_EQ,
  /// {
  MMT_LCURLY,
  /// }
  MMT_RCURLY,
  /// string
  MMT_STR,
  /// angular string <...>
  MMT_ANGSTR,
  /// $[_a..zA..Z][_a..zA..Z0..9]*
  MMT_KEYW,
  /// [_a..zA..Z][_a..zA..Z0..9]*
  MMT_IDENT,
  /// $header = none OR $header = MMT_ANGSTR OR $header = MMT_STR
  MMT_HEADERPAIR,
  /// MMT_IDENT = MMT_IDENT
  MMT_PAIR,
  /// as
  MMT_AS,
} MMTokenType;


typedef struct MMPair {
  StringView key;
  StringView val;
} MMPair;

typedef struct MMToken {
  MMTokenType type;
  Span span;
  StringView sv;
} MMToken;

typedef struct Impl {
  MMToken name;
  MMToken header;
  vec(MMPair) pairs;
} Impl;

typedef struct Interface {
  MMToken name;
  vec(MMToken) functions;
  vec(Impl*) impls;
  bool is_dynamic;
} Interface;

extern const bstr mmtok_type_to_str[];

void skip_unwanted(SrcScanner* scanner);
MMToken get_mmtok(MMParser* p);
MMToken expect_mmtok(MMParser* p, MMTokenType type);

#endif
