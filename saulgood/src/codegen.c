#include "core/stringbuilder.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "saulgood/codegen.h"
#include "saulgood/parser.h"
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
        append_sv(b, va_arg(args, StringView));
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

void gen_from_test(SGCodegen* c, Test* test) {
  appendf(&c->output, "\n// %s : %s\nvoid %s() {%s}\n", test->desc.sv, test->group.sv,
          test->name.sv, test->body.sv);
  c->test_len++;
}

void gen_from_node(SGCodegen* c, SGCodegenNode* node) {
  switch (node->type) {
  case CG_CBLOCK:
    append_sv(&c->output, node->c_code.sv);
    break;
  case CG_TEST:
    gen_from_test(c, &node->test);
    break;
  }
}

void gen_from_file(SGCodegen* c, TestFile* file) {
  for (size_t i = 0; i < file->nodes.n; i++)
    gen_from_node(c, &file->nodes.get[i]);
}

void declare_test_list(SGCodegen* c, ParserState* state) {
  // Declare list of tests
  append_str(&c->output, "struct SGTest saulgood_tests[] = {\n");
  for (size_t i = 0; i < state->files.n; i++) {
    TestFile file = state->files.get[i];
    for (size_t j = 0; j < file.nodes.n; j++) {
      SGCodegenNode node = file.nodes.get[j];
      if (node.type == CG_CBLOCK)
        continue;
      appendf(&c->output, "(struct SGTest){\"%s\", \"%s\", %s, &%s},", node.test.name.sv,
              node.test.group.sv, node.test.desc.sv, node.test.name.sv);
    }
  }
  append_str(&c->output, "\n};\n");
}

// #embed doesnt null terminate
char runner_api_code[] = {
#embed SG_RUNNER_API
    , '\0'};

void generate_code(SGCodegen* c, ParserState* state) {
  c->test_len = 0;

  // Add runner header
  append_str(&c->output, runner_api_code);

  // Generate tests
  for (size_t i = 0; i < state->files.n; i++)
    gen_from_file(c, &state->files.get[i]);

  declare_test_list(c, state);

  // Declare number of tests
  appendf(&c->output, "const int sg_test_len = %d;\n", c->test_len);
  append_ch(&c->output, '\0');
}

void codegen_destroy(SGCodegen* c) { vec_destroy(c->output); }
