#ifndef DIAG_H_
#define DIAG_H_

#include "core/diag_core.h"
#define ERRORS(X)\
X(CHASM_CLI_OUTPUT_ALR_SPECIFIED, "Output file already specified", DL_ERROR)\
X(CHASM_CLI_MULTIPLE_INPUT_FILES, "Multiple input files specified: '%s' and '%s'", DL_ERROR)\
X(CHASM_CLI_INVALID_ARGS, "Invalid argument %c", DL_ERROR)\
X(CHASM_CLI_INPUT_NOT_SPECIFIED, "Please specify an input file", DL_ERROR)\
X(CHASM_CLI_OUTPUT_NOT_SPECIFIED, "Please specify an output file (-o)", DL_ERROR)\
X(CHASM_CLI_CANNOT_OPEN_FILE, "Cannot open file '%s' for writing", DL_ERROR)\
X(CHASM_CLI_FILE_NOT_FOUND, "Input file not found: %s", DL_ERROR)

typedef enum {
  #define X(a, _, __) a,
  ERRORS(X)
  #undef X
  __chasm_diaginfos_len
} ChasmErrors;

extern const DiagInfo chasm_diaginfos[__chasm_diaginfos_len];

#endif
