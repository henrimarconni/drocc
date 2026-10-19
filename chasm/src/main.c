#include "chasm/diag.h"
#include "chu80/lexer.h"
#include "chu80/parser.h"
#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/srcman.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static void parse_args(bstr* outfile, bstr* input, CLIDiagEngine* clid) {
  char ch;
  ParsedOpt popt;
  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case 'o': {
      if (*outfile)
        clid_throw_diag(clid, CHASM_CLI_OUTPUT_ALR_SPECIFIED);
      *outfile = popt.s;
      break;
    }
    case 'h': {
      ce_printhelp();
      exit(0);
    }
    case CE_PLAIN_VALUE: {
      if (*input)
        clid_throw_diag(clid, CHASM_CLI_MULTIPLE_INPUT_FILES, *input, popt.s);
      *input = popt.s;
      break;
    }
    default:
      clid_throw_diag(clid, CHASM_CLI_INVALID_ARGS, ch);
    }
  }

  /* Validation checks AFTER parsing all arguments */
  if (!*input)
    clid_throw_diag(clid, CHASM_CLI_INPUT_NOT_SPECIFIED);
  if (!*outfile)
    clid_throw_diag(clid, CHASM_CLI_OUTPUT_NOT_SPECIFIED);
}

static int emit_output(C80Parser* p, bstr outfile, CLIDiagEngine* clid) {
  FILE* out = fopen(outfile, "w");
  if (!out) {
    clid_throw_diag(clid, CHASM_CLI_CANNOT_OPEN_FILE, outfile);
    return -1;
  }

  uint32_t i = 0;
  while (i++ < p->emitted.n)
    fputc(p->emitted.get[i - 1], out);

  fclose(out);
  return 0;
}

int main(int argc, char** argv) {
  jmp_buf onerror;
  if (setjmp(onerror) != 0) {
    return -1;
  }
  CLIDiagEngine clid = clid_engine_new(chasm_diaginfos, __chasm_diaginfos_len, &onerror);

  ce_initopt(argc, argv);
  ce_add_meta("chasm", "The 8080 Assembler", "./chasm [options] <input_file>");
  ce_addopt("output", 'o', 's', "Specify output file");
  ce_addopt("help", 'h', 0, "Print help message");

  bstr input = NULL;
  bstr output = NULL;

  parse_args(&output, &input, &clid);

  SourceManager* sman = sman_new();
  SrcScanner scanner = {0};

  if (!sman_open(&scanner, sman, input))
    clid_throw_diag(&clid, CHASM_CLI_FILE_NOT_FOUND, input);

  if (setjmp(onerror) == 0) {
    C80Lexer l = c80lex_new(scanner, sman, &onerror);
    C80Parser p = c80_new(l);
    c80_pass1(&p);
    c80_pass2(&p);
    emit_output(&p, output, &clid);
  }

  return 0;
}
