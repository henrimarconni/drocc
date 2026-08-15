#ifndef AST_H_
#define AST_H_



// #include "chucci_parse/type.h"
// typedef struct {
  
// } ExprNode;



// typedef struct {
//   TypeID tyid;
  
// } VarFnDecl;


// typedef struct {
  
// } StatementNode;


/**
  DECL -> declaration without assignment
  DEF  -> declaration + assignment
*/

typedef struct {
  enum {
    TLN_VAR_FN_DECL,
    TLN_VAR_DEF,
    TLN_FN_DEF,

    /// For struct/enum/union/typedefs
    TLN_TYPE_DEF,
  } kind; 
} TopLevelNode;


#endif



