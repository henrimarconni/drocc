#include "ce_getopt.h"
#include "codegen.h"
#include "parser.h"
#include "stringdef.h"
#include "vec.h"
#include "vmem_arena.h"
#include <assert.h>
#include <setjmp.h>

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

void write_out(bstr output_path, StringBuilder b) {
  FILE* out = output_path ? fopen(output_path, "w") : stdout;
  if (!out) {
    printf("Could not open output file %s for writing\n", output_path);
    return;
  }

  fprintf(out, "%s", b.get);

  if (output_path) {
    fclose(out);
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
  jmp_buf onerror;
  if (setjmp(onerror) == 0)
    parse_files(&state, files, &onerror);
  else {
    Codegen c = {};
    generate_code(&c, &state);
    codegen_destroy(&c);
  }
  vmarena_free(arena);
}
