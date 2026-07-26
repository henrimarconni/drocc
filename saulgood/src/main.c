#include "ce_getopt.h"
#include "codegen.h"
#include "parser.h"
#include "stringdef.h"
#include "vec.h"
#include "vmem_arena.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

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
      exit(0);
    }
    case CE_PLAIN_VALUE: {
      vec_push(*files, popt.s);
      break;
    }
    }
  }
  if (files->n == 0) {
    printf("No files specified, exiting\n");
    exit(0);
  }
}

int main(int argc, char** argv) {
  ce_initopt(argc, argv);
  ce_addopt("output", 'o', 's', "Specify output file");
  ce_addopt("directoy", 'd', 's', "Walks a directory and amalgamates all tests together");
  ce_addopt("help", 'h', 0, "Print help message");

  InputFiles files = {0};
  bstr outfile = NULL;
  parse_args(&outfile, &files);

  VMEMArena* arena = vmarena_new(128 * 1024);
  ParserState state = {};
  state.arena = arena;
  jmp_buf onerror;
  if (setjmp(onerror) == 0)
    parse_files(&state, files, &onerror);
  else
    return -1;

  Codegen c = {};
  generate_code(&c, &state);
  codegen_destroy(&c);
}
