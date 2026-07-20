#include "codegen.h"
#include "parser.h"
#include "span.h"
#include "vec.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void append_span(StringBuilder* b, Span span) {
  while (span.len--) {
    vec_push(*b, *(span.str++));
  }
}

void append_str(StringBuilder* b, bstr str) {
  while (*str != '\0') {
    vec_push(*b, *(str++));
  }
}

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
      case 't':
        append_str(b, "MM_");
        append_span(b, va_arg(args, Span));
        append_str(b, "Interface");
        break;
      case 'i':
        append_str(b, "mm_");
        append_span(b, va_arg(args, Span));
        append_str(b, "_iface");
        break;
      case 'v':
        append_str(b, "mm_");
        append_span(b, va_arg(args, Span));
        append_str(b, "_vt");
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

void append_ch(StringBuilder* b, char ch) { vec_push(*b, ch); }

#define iface_name cmd->iface->name
#define impl_name cmd->impl->name
#define impl_pairs cmd->impl->pairs

void export_static(Codegen* c, ExportCmd* cmd) {
  append_str(&c->output, "\n");
  for (size_t i = 0; i < impl_pairs.n; i++) {
    ImplKVPair* pair = &impl_pairs.get[i];
    appendf(&c->output, "#define %s %s\n", pair->key, pair->val);
  }
  append_str(&c->output, "\n\n");
}

void export_dynamic_header(Codegen* c, ExportCmd* cmd) {
  append_str(&c->output, "\n\n");

  appendf(&c->output, "struct %t {\n", iface_name);
  for (size_t i = 0; i < impl_pairs.n; i++) {
    ImplKVPair* pair = &impl_pairs.get[i];
    appendf(&c->output, "  typeof(%s)* _%s;\n", pair->val, pair->key);
  }
  append_str(&c->output, "};\n\n");

  appendf(&c->output, "extern struct %t %i;\n\n", iface_name, iface_name);

  for (size_t i = 0; i < cmd->iface->impls.n; i++) {
    Impl* impl = cmd->iface->impls.get[i];
    appendf(&c->output, "extern const struct %t %v;\n", iface_name, impl->name);
  }

  append_ch(&c->output, '\n');

  for (size_t i = 0; i < cmd->iface->functions.n; i++) {
    Span fn = cmd->iface->functions.get[i];
    appendf(&c->output, "#define %s %i._%s\n", fn, iface_name, fn);
  }
}

void export_pair_list(Codegen* c, ExportCmd* cmd, Impl* impl) {
  for (size_t i = 0; i < impl->pairs.n; i++) {
    ImplKVPair* pair = &impl->pairs.get[i];
    appendf(&c->output, "  ._%s = %s", pair->key, pair->val);
    if (i == impl->pairs.n - 1)
      append_str(&c->output, "\n};\n\n");
    else
      append_str(&c->output, ",\n");
  }
}

void export_dynamic_source(Codegen* c, ExportCmd* cmd) {
  for (size_t i = 0; i < cmd->iface->impls.n; i++) {
    Impl* impl = cmd->iface->impls.get[i];
    appendf(&c->output, "const struct %t %v = {\n", iface_name, impl->name);
    export_pair_list(c, cmd, impl);
  }
  appendf(&c->output, "struct %t %i = {\n", iface_name, iface_name);
  export_pair_list(c, cmd, cmd->impl);
}

void export_dynamic(Codegen* c, ExportCmd* cmd) {
  export_dynamic_header(c, cmd);

  appendf(&c->output, "\n\n#ifdef MM_%s_IMPLEMENTATION\n\n", iface_name);
  export_dynamic_source(c, cmd);
  append_str(&c->output, "\n#endif\n");
}

typedef vec(bstr) HideSet;
bool contains(HideSet v, Span header) {
  for (size_t i = 0; i < v.n; i++) {
    if (span_str_cmp(header, v.get[i]))
      return true;
  }
  return false;
}

void export(Codegen* c, ExportCmd* cmd) {
  // header guard
  appendf(&c->output, "#ifndef MM_%s_H__\n", iface_name);
  appendf(&c->output, "#define MM_%s_H__\n", iface_name);

  // Includes
  HideSet hideset = {0};
  for (size_t i = 0; i < cmd->iface->impls.n; i++) {
    Span header = cmd->iface->impls.get[i]->header;
    if (header.len != 0 && !contains(hideset, header)) {
      appendf(&c->output, "#include %s\n", header);
    }
    vec_push(hideset, header.str);
  }
  vec_destroy(hideset);

  // Export code
  if (cmd->iface->is_dynamic)
    export_dynamic(c, cmd);
  else
    export_static(c, cmd);

  append_str(&c->output, "#endif\n");
}

void generate_code(Codegen* c, Parser* p) {
  *c = (Codegen){0};
  c->parser = p;

  for (size_t i = 0; i < p->exports.n; i++)
    export(c, &p->exports.get[i]);

  append_ch(&c->output, '\0');
}

void codegen_destroy(Codegen* c) { vec_destroy(c->output); }
