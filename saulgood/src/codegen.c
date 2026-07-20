#include "codegen.h"
#include "parser.h"
#include "stringbuilder.h"
#include "vec.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void gen_from_test(Codegen* c, Test* test) {
  append_str(&c->output, "void ");
  append_span(&c->output, test->name);
  append_str(&c->output, "() {");
  append_span(&c->output, test->body);
  append_str(&c->output, "}\n");
}

void gen_from_node(Codegen* c, CodegenNode* node) {
  switch (node->type) {
  case CG_CBLOCK:
    append_span(&c->output, node->c_code);
    break;
  case CG_TEST:
    gen_from_test(c, &node->test);
    break;
  }
}

void gen_from_file(Codegen* c, TestFile* file) {
  for (size_t i = 0; i < file->nodes.n; i++)
    gen_from_node(c, &file->nodes.get[i]);
}

void generate_code(Codegen* c, ParserState* state) {
  for (size_t i = 0; i < state->files.n; i++)
    gen_from_file(c, &state->files.get[i]);
  append_ch(&c->output, '\0');
  printf("%s\n", c->output.get);
}
void codegen_destroy(Codegen* c) { vec_destroy(c->output); }
