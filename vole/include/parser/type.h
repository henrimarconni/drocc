#ifndef TYPE_H_
#define TYPE_H_

typedef enum {
  PT_INT,
} PrimitiveType;

typedef enum { TK_PRIMITIVE, TK_FUNCTION } TypeKind;

typedef struct {
  TypeKind kind;
  union {
    PrimitiveType pt;
  };
} VoleType;

typedef struct Parser Parser;

VoleType parse_type(Parser* p);

#endif
