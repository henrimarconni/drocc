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
      case 'd':
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", va_arg(args, int));
        append_str(b, buf);
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
  appendf(&c->output, "\n// %s : %s\nvoid %s() {%s}\n", test->desc, test->group, test->name,
          test->body);
  c->test_len++;
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

char runner_code[] = {
#embed RUNNER_SOURCE_PATH
};

char runner_header_code[] = {
#embed RUNNER_HEADER_PATH
};

void generate_code(Codegen* c, ParserState* state) {
  c->test_len = 0;

  // Add runner header
  append_str(&c->output, runner_header_code);

  // Generate tests
  for (size_t i = 0; i < state->files.n; i++)
    gen_from_file(c, &state->files.get[i]);

  // Declare list of tests
  append_str(&c->output, "struct SGTest saulgood_tests[] = {\n");
  for (size_t i = 0; i < state->files.n; i++) {
    TestFile file = state->files.get[i];
    for (size_t j = 0; j < file.nodes.n; j++) {
      CodegenNode node = file.nodes.get[j];
      if (node.type == CG_CBLOCK)
        continue;
      appendf(&c->output, "(struct SGTest){\"%s\", \"%s\", %s, &%s},", node.test.name,
              node.test.group, node.test.desc, node.test.name);
    }
  }
  append_str(&c->output, "\n};\n");
  // Declare number of tests
  appendf(&c->output, "const int sg_test_len = %d;\n", c->test_len);
  // Add runner source
  append_str(&c->output, runner_code);
  append_ch(&c->output, '\0');
  printf("%s\n", c->output.get);
}
void codegen_destroy(Codegen* c) { vec_destroy(c->output); }
