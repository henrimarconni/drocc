#ifndef TYPE_H_
#define TYPE_H_

#include "core/slice.h"
#include <assert.h>
#include <stdint.h>


typedef enum {
  TY_FUNCTION,
  TY_POINTER,
  TY_ARRAY,

  // Primitives
  TY_I8,
  TY_U8, // char,  unsigned char
  TY_I16,
  TY_U16, // short, unsigned short
  TY_I32,
  TY_U32, // int,   unsigned int
  TY_I64,
  TY_U64, // long,  unsigned long

  TY_F32,
  TY_F64, // float, double
  TY_FL,  // long double (no of bits is dependent on hardware)

  // Compound types
  TY_STRUCT,
  TY_UNION,
  TY_ENUM,

  /// forward declaration
  TY_INCOMPLETE,
  _type_kind_count
} TypeKind;

typedef struct Type Type;

static_assert(_type_kind_count < 32, "You must change struct Type's bitfields if you extend the TypeKinds beyond 32 members");
typedef struct Type {
  MemSlice payload;
  // qualifiers
  struct {
    uint8_t is_volatile : 1;
    uint8_t is_const : 1;

    /// for pointers only
    uint8_t is_restrict : 1;
    uint8_t kind : 5;
  };
} Type;

/// Storage class associated with a specific variable/function declaration/definition
typedef enum {
  SC_NONE,
  SC_TYPEDEF,
  SC_EXTERN,
  SC_STATIC,
  SC_REGISTER,
  // SC_AUTO // DEPRECATED
} StorageClass;

#endif
