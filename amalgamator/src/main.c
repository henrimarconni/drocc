#include "amalgamate.h"
#include "core/ce_getopt.h"
#include "core/stringbuilder.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static void
parse_args(volatile bstr* output_file, InputFIleVec* input_files, IncludeDirVec* include_dirs) {
  ce_add_meta(
      "amalgamator",
      "Amalgamates all #include \"...\" into a single file",
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

static void write_out(bstr output_path, StringBuilder b) {
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
  if (argc == 1)
    return -1;

  volatile bstr output_file = NULL;

  Amalgamator a = {0};

  vec_push(a.idirs, "."); // add current directory to search list
  ce_initopt(argc, argv);
  parse_args(&output_file, &a.files, &a.idirs);

  a.arena = vmarena_new(128 * 1024);
  jmp_buf onerror;

  if (setjmp(onerror) == 0) {

    StringBuilder output = amalgamate(&a, &onerror);

    if (output.get) {
      if (output_file)
        write_out(output_file, output);
      else
        printf("%s", output.get);
    }
  }
  amalgamator_free(&a);
  return 0;
}
