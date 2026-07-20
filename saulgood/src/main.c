#include "ce_getopt.h"
#include "parser.h"
#include "stringdef.h"
#include "vec.h"
#include "vmem_arena.h"
#include <assert.h>

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
    }
  }
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
  parse_files(&state, files);
}
