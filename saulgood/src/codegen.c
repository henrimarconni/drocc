#include "codegen.h"
#include "parser.h"
#include "stringbuilder.h"
#include "vec.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void appendf(StringBuilder* b, bstr fstr, ...) {
  va_list args;
  va_start(args, fstr);
  while (*fstr != '\0') {
    if (*fstr != '%')
      vec_push(*b, *(fstr++));
    else {
      switch (*++fstr) {
      case 's':
        append_span(b, va_arg(args, Span));
        break;
      default:
        printf("Invalid specifier: %c\n", *fstr);
        fflush(stdout);
        assert(false && "Invalid format specifier");
      }
      fstr++;
    }
  }
  va_end(args);
}

void gen_from_test(Codegen* c, Test* test) {
  appendf(&c->output, "void %s() {%s}\n", test->name, test->body);
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
