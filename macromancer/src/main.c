#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/fs.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "macromancer/codegen.h"
#include "macromancer/mm_arg.h"
#include "macromancer/mm_diag.h"
#include "macromancer/parser.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

static int emit_output(bstr output_path, MMCodegen* c, CLIDiagEngine* clid) {
  if (!output_path) {
    printf("%s\n", c->output.get);
    return 0;
  }
  int res = write_out(output_path, c->output.get);
  if (res < 0)
    clid_throw_diag(clid, MM_CLI_CANNOT_OPEN_FILE_FOR_WRITING, output_path);
  return 0;
}

int main(int argc, char** argv) {
  // On cli error
  jmp_buf onerror;
  if (setjmp(onerror) != 0) {
    ce_printhelp();
    return -1;
  }

  // Argument parsing
  CLIDiagEngine clid = clid_engine_new(mm_diaginfos, __mm_diagtype_len, &onerror);
  bstr confpath = NULL;
  bstr output_path = NULL;
  ExportOverrideVec export_overrides = {};
  parse_arg(argc, argv, &output_path, &confpath, &export_overrides, &clid);

  // Setup
  MMParser p;
  SourceManager* sman = sman_new();
  VMEMArena* arena = vmarena_new(1024 * 128);

  // Parse
  if (setjmp(onerror) == 0)
    read_conf(&p, sman, arena, confpath, &export_overrides, &onerror);
  else {
    vec_destroy(export_overrides);
    parser_destroy(&p);
    vmarena_free(arena);
    return -1;
  }

  MMCodegen c;
  generate_code(&c, &p);

  int res = emit_output(output_path, &c, &clid);

  // Cleanup
  vec_destroy(export_overrides);
  codegen_destroy(&c);
  parser_destroy(&p);
  vmarena_free(arena);
  return res;
}
