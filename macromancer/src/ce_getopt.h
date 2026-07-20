#ifndef ARGP_H
#define ARGP_H

#include "stringdef.h"
#include <stdbool.h>
#include <stddef.h>

#define CE_PLAIN_VALUE -1


typedef struct {
  bstr longhand;
  char shorthand;  // MUST be unique
  char val_format; // f, d, s, 0 for no value
  bstr desc;
} Opt;

typedef union {
  bool flag;
  // 1-character names used because they are
  // already known to user
  ostr s; 
  int d;   
  float f; 
} ParsedOpt;

// ch is the shorthand name of the option
void ce_initopt(int argc, char** argv);
void ce_addopt(bstr longhand, char shorthand, char val_format, bstr desc);
bool ce_getopt(char* ch, ParsedOpt* popt);
void ce_printhelp();

#endif
