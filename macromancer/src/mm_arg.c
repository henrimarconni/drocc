#include "ce_getopt.h"
#include "cli_diag.h"
#include "mm_arg.h"
#include "parser.h"
#include "stringdef.h"
#include "vec.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

inline static void handle_arg(char ch, ParsedOpt popt, bstr* output_path, bstr* confpath,
                              ExportOverrideVec* overrides) {
  switch (ch) {
  case 'o': {
    if (*output_path) {
      vec_destroy(*overrides);
      clid_throw_diag(CLID_ERROR, MMCLI_ERR_OUTPUT_FLAG_MISUSE,
                      "Output flag can only be used once");
    }
    *output_path = popt.s;
    break;
  }
  case 'h': {
    ce_printhelp();
    exit(0);
  }
  case 'e': {
    ExportOverride ov;
    bstr str = popt.s;
    ov.iface = str;

    while (*str != '\0' && *str != '=')
      str++;
    if (*str == '\0') {
      vec_destroy(*overrides);
      clid_throw_diag(CLID_ERROR, MMCLI_ERR_INVALID_EXPORT_CMD,
                      "Expected `--export interface=implementation`, found `--export %s`\n",
                      ov.iface);
    }
    *str = '\0';
    ov.impl = str + 1;

    vec_push(*overrides, ov);
    break;
  }
  case CE_PLAIN_VALUE: {
    if (*confpath) {
      vec_destroy(*overrides);
      clid_throw_diag(CLID_ERROR, MMCLI_ERR_MULTIPLE_INPUT_FILES,
                      "Input file already specified: %s, cannot overwrite it with: %s\n", *confpath,
                      popt.s);
    }
    *confpath = popt.s;
    break;
  }

  default:
    assert(false && "Unreachable");
  }
}

void parse_arg(int argc, char** argv, bstr* output_path, bstr* confpath,
               ExportOverrideVec* overrides) {
  ce_initopt(argc, argv);
  ce_add_meta("macromancer", "A code generator that adds compile-time and runtime interfaces to C",
              "./macromancer file.mm -o output.h --export Interface=Implementation");
  ce_addopt("output", 'o', 's', "Output file.h location");
  ce_addopt("help", 'h', 0, "Print help message");
  ce_addopt("export", 'e', 's',
            "Export interface=implementation (overrides the exports written in file)");
  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt))
    handle_arg(ch, popt, output_path, confpath, overrides);
  if (!*confpath) {
    vec_destroy(*overrides);
    clid_throw_diag(CLID_ERROR, MMCLI_ERR_INPUT_FILE_NOT_SPECIFIED, "Input file not specified\n");
  }
}
