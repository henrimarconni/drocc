#ifndef MM_ARG_H
#define MM_ARG_H

#include "core/stringdef.h"
#include "macromancer/parser.h"

typedef enum {
  MMCLI_ERR_OUTPUT_FLAG_MISUSE = -1,
  MMCLI_ERR_INVALID_EXPORT_CMD = -2,
  MMCLI_ERR_INPUT_FILE_NOT_SPECIFIED = -3,
  MMCLI_ERR_MULTIPLE_INPUT_FILES = -4,
  MMCLI_ERR_CANT_OPEN_OUTFILE = -5,
} MMCliError;

void parse_arg(int argc, char** argv, bstr* output_path, bstr* confpath,
               ExportOverrideVec* overrides);

#endif
