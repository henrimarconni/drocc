#include "cli_diag.h"
#include "span.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void print_level(CLIDiagLevel level) {
  switch (level) {
  case CLID_ERROR:
    printf(ANSI_RED "Error: " ANSI_RESET);
    break;
  case CLID_NOTE:
    printf(ANSI_BLUE "Note: " ANSI_RESET);
    break;
  case CLID_WARN:
    printf(ANSI_YELLOW "Warning: " ANSI_RESET);
    break;
  }
}

void clid_print_diag(CLIDiagLevel level, bstr msg, ...) {
  va_list args;
  va_start(args, msg);
  print_level(level);
  vprintf(msg, args);
  putchar('\n');
  va_end(args);
}

[[noreturn]]
void clid_throw_diag(CLIDiagLevel level, int error_id, bstr msg, ...) {
  va_list args;
  va_start(args, msg);
  print_level(level);
  vprintf(msg, args);
  putchar('\n');
  va_end(args);
  exit(error_id);
}
