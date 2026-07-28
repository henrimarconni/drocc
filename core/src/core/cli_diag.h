/**
  @file
  @brief Provides a lightweight utility to throw consistent diagnostics related to CLI
  * 
 */


#ifndef CLI_DIAG_H
#define CLI_DIAG_H

#include "core/stringdef.h"


typedef enum {
  CLID_ERROR,
  CLID_NOTE,
  CLID_WARN
} CLIDiagLevel;


/**
  Terminate the program ( exit(error_id) ) after printing diagnostic
  @param level Diagnostic level
  @param error_id Error ID related to the error
  @param msg Error message (can contain printf style format specifiers)
*/
[[noreturn]]
void clid_throw_diag(CLIDiagLevel level, int error_id, bstr msg, ...);

/**
  Print diagnostic
  @param level Diagnostic level
  @param msg Error message (can contain printf style format specifiers)
*/
void clid_print_diag(CLIDiagLevel level, bstr msg, ...);

#endif
