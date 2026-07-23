#include "amalgamate.h"
#include "ce_getopt.h"
#include "stringbuilder.h"
#include "vec.h"
#include "vmem_arena.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

void parse_args(bstr* output_file, InputFIleVec* input_files, IncludeDirVec* include_dirs) {
  // TODO: Make it scan all #includes and recursively expand them
  // ce_add_meta("amalgamator", "Amalgamates all #include \"...\" into a single file",
  //             "./amalgamator file.c -o output.c -I includes/");
  ce_add_meta("amalgamator", "Concatenates multiple source files into a single output file.",
              "./amalgamator file1.c file2.c file3.c -o amalgamated.c -I ./code_dir");
  ce_addopt("help", 'h', 0, "Print help");
  ce_addopt("output", 'o', 's', "Specify output location");
  ce_addopt("include-dir", 'I', 's', "Specify a location to find included files");

  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'h':
      ce_printhelp();
      exit(0);
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
      vec_push(*input_files, popt.s);
      break;
    }
  }
  if (input_files->n == 0) {
    printf("Error: no files specified\n");
    exit(-1);
  }
}

int main(int argc, char** argv) {
  if (argc == 1)
    return -1;
  bstr output_file = NULL;
  InputFIleVec input_files = {};
  IncludeDirVec include_dirs = {};
  vec_push(include_dirs, "."); // add current directory to search list
  ce_initopt(argc, argv);
  parse_args(&output_file, &input_files, &include_dirs);

  VMEMArena* arena = vmarena_new(128 * 1024);
  jmp_buf onerror;
  if (setjmp(onerror) == 0) {
    StringBuilder output = amalgamate(arena, include_dirs, input_files, &onerror);
    if (output.get)
      printf("%s\n", output.get);
    vec_destroy(output);
  }
  vec_destroy(include_dirs);
  vmarena_free(arena);
  return 0;
}
