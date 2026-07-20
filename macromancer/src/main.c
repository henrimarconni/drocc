#include "ce_getopt.h"
#include "codegen.h"
#include "diagnostics.h"
#include "parser.h"
#include "stringdef.h"
#include "vec.h"
#include "vmem_arena.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define print_err(fstr, ...)                                                                       \
  do {                                                                                             \
    fprintf(stderr, ERROR_STR fstr __VA_OPT__(, ) __VA_ARGS__);                                    \
  } while (0)

void parse_arg(int argc, char** argv, bstr* output_path, bstr* confpath,
               ExportOverrideVec* overrides) {
  ce_initopt(argc, argv);
  ce_addopt("output", 'o', 's', "Output file.h location");
  ce_addopt("help", 'h', 0, "Print help message");
  ce_addopt("export", 'e', 's',
            "Export interface=implementation (overrides the exports written in file)");
  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'o': {
      if (*output_path) {
        print_err("--output flag used 2+ times\n");
        exit(-1);
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
        print_err("expected `--export interface=implementation`, found `--export %s`\n", ov.iface);
        exit(-1);
      }
      *str = '\0';
      ov.impl = str + 1;

      vec_push(*overrides, ov);
      break;
    }
    case CE_PLAIN_VALUE: {
      if (*confpath) {
        print_err("input file already specified: %s, cannot overwrite it with: %s\n", *confpath,
                  popt.s);
        exit(-1);
      }
      *confpath = popt.s;
      break;
    }

    default:
      assert(false && "Unreachable");
    }
  }
  if (!*confpath) {
    print_err("input file not specified\n");
    exit(-1);
  }
}

void write_out(bstr output_path, Codegen* c) {
  FILE* out = output_path ? fopen(output_path, "w") : stdout;
  if (!out) {
    print_err("Could not open output file %s for writing\n", output_path);
    exit(-1);
  }

  fprintf(out, "%s\n", c->output.get);

  if (output_path) {
    fclose(out);
  }
}

int main(int argc, char** argv) {
  bstr confpath = NULL;
  bstr output_path = NULL;
  ExportOverrideVec export_overrides = {};

  parse_arg(argc, argv, &output_path, &confpath, &export_overrides);

  jmp_buf onerror;
  Parser p;
  VMEMArena* arena = vmarena_new(1024 * 128); // max size is 128 kb

  if (setjmp(onerror) == 0) {
    read_conf(&p, confpath, &export_overrides, arena, &onerror);
  } else {
    printf("Parsing failed, exiting\n");
    if (export_overrides.get)
      vec_destroy(export_overrides);
    parser_destroy(&p);
    vmarena_free(arena);
    return -1;
  }

  Codegen c;
  generate_code(&c, &p);
  write_out(output_path, &c);

  if (export_overrides.get)
    vec_destroy(export_overrides);
  codegen_destroy(&c);
  parser_destroy(&p);
  vmarena_free(arena);
  return 0;
}
