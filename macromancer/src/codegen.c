#include "core/stringdef.h"
#include "core/strutils.h"
#include "core/vec.h"
#include "macromancer/codegen.h"
#include "macromancer/mmtok.h"
#include "macromancer/parser.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void appendf(StringBuilder* b, bstr fstr, ...) {
  va_list args;
  va_start(args, fstr);
  while (*fstr != '\0') {
    if (*fstr != '%') {
      vec_push(*b, *(fstr++));
    } else {
      if (*++fstr == '\0') {
        assert(false && "Format string ends with a dangling '%'");
        break;
      }

      switch (*fstr) {
      case 's': {
        StringView sv = va_arg(args, StringView);
        append_sv(b, sv);
        break;
      }
      case 't': {
        append_str(b, "MM_");
        StringView sv = va_arg(args, StringView);
        append_sv(b, sv);
        append_str(b, "Interface");
        break;
      }
      case 'i': {
        append_str(b, "mm_");
        StringView sv = va_arg(args, StringView);
        append_sv(b, sv);
        append_str(b, "_iface");
        break;
      }
      case 'v': {
        append_str(b, "mm_");
        StringView sv = va_arg(args, StringView);
        append_sv(b, sv);
        append_str(b, "_vt");
        break;
      }
      default: {
        printf("Invalid specifier: %c\n", *fstr);
        fflush(stdout);
        assert(false && "Invalid format specifier");
      }
      }
      fstr++;
    }
  }
  va_end(args);
}

#define iface_name cmd->iface->name.sv
#define impl_name cmd->impl->name.sv
#define impl_pairs cmd->impl->pairs

static void export_static(MMCodegen* c, ExportCmd* cmd) {
  append_str(&c->output, "\n");
  for (size_t i = 0; i < impl_pairs.n; i++) {
    MMPair* pair = &impl_pairs.get[i];
    appendf(&c->output, "#define %s %s\n", pair->key, pair->val);
  }
  append_str(&c->output, "\n\n");
}

static void export_dynamic_header(MMCodegen* c, ExportCmd* cmd) {
  append_str(&c->output, "\n\n");

  appendf(&c->output, "struct %t {\n", iface_name);
  for (size_t i = 0; i < impl_pairs.n; i++) {
    MMPair* pair = &impl_pairs.get[i];
    appendf(&c->output, "  typeof(%s)* _%s;\n", pair->val, pair->key);
  }
  append_str(&c->output, "};\n\n");

  appendf(&c->output, "extern struct %t %i;\n\n", iface_name, iface_name);

  for (size_t i = 0; i < cmd->iface->impls.n; i++) {
    Impl* impl = cmd->iface->impls.get[i];
    appendf(&c->output, "extern const struct %t %v;\n", iface_name, impl->name.sv);
  }

  append_ch(&c->output, '\n');

  for (size_t i = 0; i < cmd->iface->functions.n; i++) {
    StringView fn = cmd->iface->functions.get[i].sv;
    appendf(&c->output, "#define %s %i._%s\n", fn, iface_name, fn);
  }
}

static void export_pair_list(MMCodegen* c, Impl* impl) {
  for (size_t i = 0; i < impl->pairs.n; i++) {
    MMPair* pair = &impl->pairs.get[i];
    appendf(&c->output, "  ._%s = %s", pair->key, pair->val);
    if (i == impl->pairs.n - 1)
      append_str(&c->output, "\n};\n\n");
    else
      append_str(&c->output, ",\n");
  }
}

static void export_dynamic_source(MMCodegen* c, ExportCmd* cmd) {
  for (size_t i = 0; i < cmd->iface->impls.n; i++) {
    Impl* impl = cmd->iface->impls.get[i];
    appendf(&c->output, "const struct %t %v = {\n", iface_name, impl->name.sv);
    export_pair_list(c, impl);
  }
  appendf(&c->output, "struct %t %i = {\n", iface_name, iface_name);
  export_pair_list(c, cmd->impl);
}

static void export_dynamic(MMCodegen* c, ExportCmd* cmd) {
  export_dynamic_header(c, cmd);

  appendf(&c->output, "\n\n#ifdef MM_%s_IMPLEMENTATION\n\n", iface_name);
  export_dynamic_source(c, cmd);
  append_str(&c->output, "\n#endif\n");
}

typedef vec(StringView) HideSet;
static bool contains(HideSet v, StringView header) {
  for (size_t i = 0; i < v.n; i++) {
    if (sv_cmp(header, v.get[i]))
      return true;
  }
  return false;
}

static void export(MMCodegen* c, ExportCmd* cmd) {
  // header guard
  appendf(&c->output, "#ifndef MM_%s_H__\n", iface_name);
  appendf(&c->output, "#define MM_%s_H__\n", iface_name);

  // Includes
  HideSet hideset = {0};
  for (size_t i = 0; i < cmd->iface->impls.n; i++) {
    StringView header = cmd->iface->impls.get[i]->header.sv;

    if (header.len != 0 && !contains(hideset, header)) {
      appendf(&c->output, "#include %s\n", header);
      vec_push(hideset, header);
    }
  }
  vec_destroy(hideset);

  // Export code
  if (cmd->iface->is_dynamic)
    export_dynamic(c, cmd);
  else
    export_static(c, cmd);

  append_str(&c->output, "#endif\n");
}

void generate_code(MMCodegen* c, MMParser* p) {
  *c = (MMCodegen){0};
  c->parser = p;

  for (size_t i = 0; i < p->exports.n; i++)
    export(c, &p->exports.get[i]);

  append_ch(&c->output, '\0');
}

void codegen_destroy(MMCodegen* c) { vec_destroy(c->output); }

#undef iface_name
#undef impl_name
#undef impl_pairs
