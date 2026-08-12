#ifndef DECL_SPEC_H_
#define DECL_SPEC_H_

#include "chucci_parse/parser.h"
#include "chucci_parse/type.h"

typedef struct {
  // Primitives
  int num_void, num_bool, num_char, num_short, num_int, num_long;
  int num_float, num_double, num_signed, num_unsigned;
  int total_primitives;

  // Qualifiers
  bool is_const;
  bool is_volatile;
  bool is_restrict;

  // Storage Class
  StorageClass storage_class;
} DeclSpecBuilder;

void parse_decl_specifier(Parser* p, TypeID* tyid, StorageClass* sc);

#endif
