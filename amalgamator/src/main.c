#include "amalgamate.h"
#include "ce_getopt.h"
#include "stringbuilder.h"
#include "vec.h"
#include "vmem_arena.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

void parse_args(bstr* output_file, bstr* input_file, IncludeDirVec* include_dirs) {
  ce_add_meta("amalgamator", "Amalgamates all #include \"...\" into a single file",
              "./amalgamator file.c -o output.c -I includes/");
  ce_addopt("help", 'h', 0, "Print help");
  ce_addopt("output", 'o', 's', "Specify output location");
  ce_addopt("include-dir", 'I', 's', "Specify a location to find included files");

  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'h':
      ce_printhelp();
      break;
    case 'o':
      if (*output_file) {
        printf("Error: output file already specified\n");
        exit(-1);
      }
      *output_file = popt.s;
      break;
    case 'I':
      vec_push(*include_dirs, popt.s);
      break;
    case CE_PLAIN_VALUE:
      if (*input_file) {
        printf("Error: input file already specified\n");
        exit(-1);
      }
      *input_file = popt.s;
    }
  }
}

int main(int argc, char** argv) {
  if (argc == 1)
    return -1;
  bstr output_file = NULL;
  bstr input_file = NULL;
  IncludeDirVec include_dirs = {};
  vec_push(include_dirs, "."); // add current directory to search list
  ce_initopt(argc, argv);
  parse_args(&output_file, &input_file, &include_dirs);

  VMEMArena* arena = vmarena_new(128 * 1024);
  jmp_buf onerror;
  StringBuilder output;
  if (setjmp(onerror) == 0)
    output = amalgamate(arena, include_dirs, output_file, input_file, &onerror);

  vec_destroy(output);
  vec_destroy(include_dirs);
  vmarena_free(arena);
  return 0;
}
