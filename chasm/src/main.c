#include "chu80/lexer.h"
#include "chu80/parser.h"
#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/srcman.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static void parse_args(bstr* outfile, bstr* input) {
  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'o': {
      if (*outfile)
        clid_throw_diag(CLID_ERROR, -1, "Output file already specified");
      *outfile = popt.s;
      break;
    }
    case 'h': {
      ce_printhelp();
      exit(0);
    }
    case CE_PLAIN_VALUE: {
      if (*input)
        clid_throw_diag(
            CLID_ERROR, -1, "Multiple input files specified: '%s' and '%s'", *input, popt.s);
      *input = popt.s;
      break;
    }
    default:
      clid_throw_diag(CLID_ERROR, -1, "Invalid argument %c", ch);
    }
  }

  /* Validation checks AFTER parsing all arguments */
  if (!*input)
    clid_throw_diag(CLID_ERROR, -1, "Please specify an input file");
  if (!*outfile)
    clid_throw_diag(CLID_ERROR, -1, "Please specify an output file (-o)");
}

static int emit_output(C80Parser* p, bstr outfile) {
  FILE* out = fopen(outfile, "w");
  if (!out) {
    clid_throw_diag(CLID_ERROR, -1, "Cannot open file '%s' for writing", outfile);
    return -1;
  }

  uint32_t i = 0;
  while (i++ < p->emitted.n)
    fputc(p->emitted.get[i - 1], out);

  fclose(out);
  return 0;
}

int main(int argc, char** argv) {
  ce_initopt(argc, argv);
  ce_add_meta("chu80", "Z80 C Compiler", "chu80 [options] <input_file>");
  ce_addopt("output", 'o', 's', "Specify output file");
  ce_addopt("help", 'h', 0, "Print help message");

  bstr input = NULL;
  bstr output = NULL;

  parse_args(&output, &input);

  SourceManager* sman = sman_new();
  SrcScanner scanner = {0};

  if (!sman_open(&scanner, sman, input))
    clid_throw_diag(CLID_ERROR, -1, "Input file not found: %s", input);

  jmp_buf onerror;

  if (setjmp(onerror) == 0) {
    C80Lexer l = c80lex_new(scanner, sman, &onerror);
    C80Parser p = c80_new(l);
    c80_pass1(&p);
    c80_pass2(&p);
    emit_output(&p, output);
  }

  return 0;
}
