#ifndef TOPDECL_H_
#define TOPDECL_H_

#include "core/string_interner.h"
#include "core/vec.h"
#include "parser/type.h"
typedef enum {
  TD_FUNC,
} TopDeclKind; 

typedef struct Stmt Stmt;

typedef struct {
  VoleType ret;
  vec(Stmt) stmts;
  InternID name;
} FuncDef;

typedef struct {
  TopDeclKind kind;
  union {
    FuncDef fndef;
  };
} TopDecl;

#endif
