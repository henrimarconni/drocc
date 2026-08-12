#ifndef DECLARATOR_H_
#define DECLARATOR_H_

#include "core/string_interner.h"

typedef struct Declarator Declarator;

typedef enum {
  DECL_POINTER,
  DECL_FUNCTION,
  DECL_ARRAY,
  DECL_IDENT
} DeclaratorKind;

/// Temporary object used to form a declarator type
typedef struct Declarator {
  Declarator* inner;
  DeclaratorKind kind;

  union {
    InternID ident;
  };
} Declarator;

#endif
