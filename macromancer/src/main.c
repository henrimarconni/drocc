#include "core/cli_diag.h"
#include "core/fs.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "macromancer/codegen.h"
#include "macromancer/mm_arg.h"
#include "macromancer/parser.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

int emit_output(bstr output_path, Codegen* c) {
  if (!output_path) {
    printf("%s\n", c->output.get);
    return 0;
  }
  int res = write_out(output_path, c->output.get);
  if (res < 0) {
    clid_print_diag(CLID_ERROR, "Cannot open file: `%s` for writing", output_path);
    return MMCLI_ERR_CANT_OPEN_OUTFILE;
  }
  return 0;
}

int main(int argc, char** argv) {
  bstr confpath = NULL;
  bstr output_path = NULL;
  ExportOverrideVec export_overrides = {};

  parse_arg(argc, argv, &output_path, &confpath, &export_overrides);

  jmp_buf onerror;
  Parser p;
  VMEMArena* arena = vmarena_new(1024 * 128); // max size is 128 kb

  if (setjmp(onerror) == 0)
    read_conf(&p, confpath, &export_overrides, arena, &onerror);
  else {
    vec_destroy(export_overrides);
    parser_destroy(&p);
    vmarena_free(arena);
    return -1;
  }

  Codegen c;
  generate_code(&c, &p);
  int res = emit_output(output_path, &c);
  vec_destroy(export_overrides);
  codegen_destroy(&c);
  parser_destroy(&p);
  vmarena_free(arena);
  return res;
}
