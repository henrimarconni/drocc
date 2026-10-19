/**
  @file
  Clean and lightweight diagnostics engine
*/

#ifndef DIAG_CORE_H_
#define DIAG_CORE_H_

#include "core/srcman.h"
#include "core/stringdef.h"
#include "core/stringdef.h"
#include <stddef.h>
#include <stdio.h>

#define ERROR_STR ANSI_RED "Error: " ANSI_RESET
#define WARN_STR ANSI_YELLOW "Error: " ANSI_RESET
#define NOTE_STR ANSI_BLUE "Note: " ANSI_RESET

/**
  Format specifiers X-macro, containing all the format specifiers
*/
#define FORMAT_SPECS(X)                                                                            \
  X("%sv", print_sv, StringView)                                                                   \
  X("%s", print_str, bstr)                                                                         \
  X("%d", print_int, int)                                                                          \
  X("%c", putchar, int)

typedef enum {
  DL_NOTE,
  DL_WARNING,
  DL_ERROR,
} DiagLevel;

/**
  Contains all information about diagnostics
*/
typedef struct {
  DiagLevel level;

  /**
    msg can contain format specifiers as defined in FORMAT_SPECS(X)
    @see FORMAT_SPECS
  */
  bstr msg;
} DiagInfo;

void print_diag_intro(DiagLevel level);
void diag_vprintf(bstr str, va_list args);

#endif
