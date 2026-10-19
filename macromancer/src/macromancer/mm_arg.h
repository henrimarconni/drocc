#ifndef MM_ARG_H
#define MM_ARG_H

#include "core/cli_diag.h"
#include "core/stringdef.h"
#include "macromancer/parser.h"

void parse_arg(int argc, char** argv, bstr* output_path, bstr* confpath,
               ExportOverrideVec* overrides, CLIDiagEngine* clid);
#endif
