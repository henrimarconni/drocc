#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "chucci_parse/type.h"
#include <stdint.h>

typedef struct {
  // Is it a normal symbol or a typedef
  bool is_typedef;
  TypeID type;
} Symbol;

#endif
