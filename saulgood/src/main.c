#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/fs.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "saulgood/codegen.h"
#include "saulgood/parser.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

typedef enum { SGCLI_INVALID_ARG = -1, SGCLI_FILE_DOESNT_EXIST = -2 } SGCLIError;

void parse_args(bstr* outfile, InputFiles* files) {
  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'o': {
      *outfile = popt.s;
      break;
    }
    case 'h': {
      ce_printhelp();
      break;
    }
    case CE_PLAIN_VALUE: {
      vec_push(*files, popt.s);
      break;
    }
    default:
      clid_throw_diag(CLID_ERROR, SGCLI_INVALID_ARG, "Invalid argument %c", ch);
    }
  }
  if (files->n == 0)
    exit(0);
}

int emit_output(Codegen* c, bstr outfile) {
  if (!outfile) {
    printf("%s\n", c->output.get);
    return 0;
  }
  int res = write_out(outfile, c->output.get);
  if (res < 0) {
    clid_print_diag(CLID_ERROR, "Cannot open file: %s for writing", outfile);
  }
  return 0;
}

int main(int argc, char** argv) {
  ce_initopt(argc, argv);
  ce_addopt("output", 'o', 's', "Specify output file");
  ce_addopt("directoy", 'd', 's', "Walks a directory and amalgamates all tests together");
  ce_addopt("help", 'h', 0, "Print help message");

  InputFiles files = {};
  bstr outfile = NULL;

  parse_args(&outfile, &files);

  VMEMArena* arena = vmarena_new(128 * 1024);
  ParserState state = {};
  state.arena = arena;
  jmp_buf onerror;

  if (setjmp(onerror) == 0) {
    parse_files(&state, files, &onerror);
    Codegen c = {};
    generate_code(&c, &state);
    emit_output(&c, outfile);
    codegen_destroy(&c);
  }
  parser_free(&state);
  vmarena_free(arena);
  vec_destroy(files);
}
