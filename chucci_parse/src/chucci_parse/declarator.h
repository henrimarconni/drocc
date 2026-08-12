#ifndef DECLARATOR_H_
#define DECLARATOR_H_

#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"
#include "core/string_interner.h"
#include "core/vec.h"

typedef struct Declarator Declarator;

typedef enum {
  DECL_POINTER,
  DECL_FUNCTION,
  DECL_ARRAY,
  DECL_INCOMPLETE_ARRAY,
  DECL_IDENT
} DeclaratorKind;

/// Temporary object used to form a declarator type
typedef struct Declarator {
  Declarator* inner;
  DeclaratorKind kind;

  union {
    InternID ident;
    vec(TypeID) params;
    TyQualifier ptrqual; //< for pointers
  };
} Declarator;

void print_decl(Parser* p, Declarator* decl);
Declarator* parse_declarator(Parser* p);
TypeID unwind_declarator(Declarator* decl, Parser* p, TypeID current);

#endif
