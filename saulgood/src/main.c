#include "ce_getopt.h"
#include "cli_diag.h"
#include "codegen.h"
#include "parser.h"
#include "stringdef.h"
#include "utils.h"
#include "vec.h"
#include "vmem_arena.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>

typedef enum : int { SGCLI_INVALID_ARG = -1, SGCLI_FILE_DOESNT_EXIST = -2 } SGCLIError;

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

void emit_output(Codegen* c, bstr outfile) {
  if (outfile) {
    int res = write_out(outfile, c->output.get);
    if (res < 0)
      clid_throw_diag(CLID_ERROR, SGCLI_FILE_DOESNT_EXIST, "File %s doesnt exist", outfile);
  } else
    printf("%s\n", c->output.get);
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
