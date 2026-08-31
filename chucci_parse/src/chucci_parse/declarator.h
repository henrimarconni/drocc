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
    /**
      ident is optional (only required in function definition but not in declaration)
      If no ident was parsed then ident = 0
      [Ident] [TypeID] [Ident] [TypeID] .... [Return TypeID]
    */
    vec(uint32_t) params;
    TyQualifier ptrqual; //< for pointers
  };
} Declarator;

void print_decl(Parser* p, Declarator* decl);
Declarator* parse_declarator(Parser* p);

/// Outputs TypeID of the unwinded type and name (if exists) associated with the declarator
void unwind_declarator(TypeID* tyid, InternID* name, Declarator* decl, Parser* p, TypeID current);

#endif
