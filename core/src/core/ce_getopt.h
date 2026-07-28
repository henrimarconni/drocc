/**
  @file
  A clean and minimal command line argument parser utility, inspired by posix getopt
  but adds a cleaner and faster handling using "shorthands" and is cross platform
*/


#ifndef ARGP_H
#define ARGP_H

#include "core/stringdef.h"
#include <stdbool.h>
#include <stddef.h>

#define CE_PLAIN_VALUE -1

/// @brief This struct contains data related to a single command
typedef struct {
  
  /// longhand of the command (example: --output)
  /// @note Omit the --, longhand = "output" not "--output"
  bstr longhand;

  /// shorthand of the command (example: -o)
  /// @note MUST be unique
  /// @note Omit the -, shorthand = 'o'
  char shorthand;

  /// 'f' for float, 'd' for integer, 's' for string, 0 for no value
  /// @note Any other value except the above will cause an assert
  char val_format;

  /// description related to the command
  bstr desc;

} Opt;

/// This contains the parsed data for the related command
typedef union {
  bool flag;
  ostr s;
  int d;   
  float f; 
} ParsedOpt;

/// @note Must be called before ce_getopt.
void ce_initopt(int argc, char** argv);
void ce_add_meta(bstr name, bstr desc, bstr usage);
void ce_addopt(bstr longhand, char shorthand, char val_format, bstr desc);
/**
  Sets ch to the shorthand of the next parsed character
  Sets popt to the parsed data for the next parsed command
  @return false if the argument list is exhausted or it cannot continue, true otherwise
  Usage Example:
  @code
  ce_initopt(argc, argv);
  ce_addopt("output", 'o', 's', "Specify output file");
  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    if (ch == 'o') {
      printf("Outputting to file: %s\n", popt.s); 
    }
  }
  @endcode
*/
bool ce_getopt(char* ch, ParsedOpt* popt);
void ce_printhelp();

#endif
