#include "diagnostics.h"
#include "mm_diag.h"
#include "parser.h"
#include "scanner.h"
#include "span.h"
#include "vec.h"
#include "vmem_arena.h"
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_ID_LEN 128

#define DISPATCH_TABLE(X)                                                                          \
  X("$impl", parse_impl)                                                                           \
  X("$interface", parse_interface)                                                                 \
  X("$export", parse_export)

void skip_comment(SourceFile* file) {
  if (peekch(file) == '#') {
    while (peekch(file) != '\n' && peekch(file) != EOF) {
      nextch(file);
    }
  }
}

void skip_unwanted(SourceFile* file) {
  int last_id;
  do {
    last_id = file->pos.id;
    skip_comment(file);
    skip_space(file);
  } while (file->pos.id != last_id && peekch(file) != EOF);
}

Span get_tok(Parser* p) {
  skip_unwanted(&p->file);

  Span span = span_begin(&p->file);
  int ch = peekch(&p->file);
  if (ch == EOF)
    throw_diag(&p->engine, span, MM_ERR_UNEXPECTED_EOF, span);

  if (ch == '{' || ch == '}' || ch == '=') {
    nextch(&p->file);
    span_end(&span);
    skip_unwanted(&p->file);
    return span;
  }

  while (ch != EOF && !isspace(ch) && ch != '{' && ch != '}' && ch != '=') {
    nextch(&p->file);
    ch = peekch(&p->file);
  }
  span_end(&span);
  skip_unwanted(&p->file);
  return span;
}

void expect(Parser* p, bstr str) {
  Span span = get_tok(p);
  if (!span_str_cmp(span, str))
    throw_diag(&p->engine, span, MM_ERR_UNEXPECTED_TOK, str, span);
}

void add_interface(Parser* p, Interface interface) {
  for (size_t i = 0; i < p->interfaces.n; i++) {
    if (span_cmp(interface.name, p->interfaces.get[i]->name))
      throw_diag(&p->engine, interface.name, MM_ERR_INTERFACE_ALREADY_EXISTS, interface.name);
  }
  Interface* mem = vmarena_alloc(p->arena, sizeof(Interface));
  *mem = interface;
  vec_push(p->interfaces, mem);
}

void parse_interface(Parser* p) {
  Span name = get_tok(p);
  expect(p, "as");
  Span type = get_tok(p);

  bool is_dynamic;
  if (span_str_cmp(type, "Dynamic"))
    is_dynamic = true;
  else if (span_str_cmp(type, "Static"))
    is_dynamic = false;
  else
    throw_diag(&p->engine, type, MM_ERR_UNEXPECTED_TOK, "`Dynamic` or `Static`", type);

  expect(p, "{");

  Interface iface = {0};
  iface.is_dynamic = is_dynamic;
  iface.name = name;

  while (true) {
    Span tok = get_tok(p);
    if (tok.len == 1 && *tok.str == '}')
      break;
    vec_push(iface.functions, tok);
  }

  add_interface(p, iface);
}

#define is_valid_id_char(ch)                                                                       \
  (isalnum((unsigned char)(ch)) || (ch) == '=' || (ch) == '"' || (ch) == '$' || (ch) == '.' ||     \
   (ch) == '_')

Span expect_str(Parser* p) {
  skip_unwanted(&p->file);
  Span span = span_begin(&p->file);
  char ch = nextch(&p->file);
  if (ch != '"')
    throw_diag(&p->engine, span, MM_ERR_INVALID_STRING, span);
  while (ch != '"' && ch != EOF)
    ch = nextch(&p->file);
  if (ch != '"')
    throw_diag(&p->engine, span, MM_ERR_INVALID_STRING, span);
  span_end(&span);
  return span;
}

void parse_pair(Parser* p, ImplKVPair* pair) {
  pair->key = get_tok(p);
  expect(p, "=");
  pair->val = get_tok(p);
}

Impl* find_impl(Parser* p, Interface* iface, Span name) {
  for (size_t i = 0; i < iface->impls.n; i++) {
    Impl* impl = iface->impls.get[i];
    if (span_cmp(impl->name, name))
      return impl;
  }
  return NULL;
}

ssize_t find_iface_idx(Parser* p, Span name) {
  for (size_t i = 0; i < p->interfaces.n; i++) {
    if (span_cmp(p->interfaces.get[i]->name, name))
      return (ssize_t)i;
  }
  return -1;
}

Interface* find_iface(Parser* p, Span name) {
  ssize_t idx = find_iface_idx(p, name);
  return (idx >= 0) ? p->interfaces.get[idx] : NULL;
}

int find_fn_id(Parser* p, Interface* iface, Span name) {
  for (size_t i = 0; i < iface->functions.n; i++) {
    if (span_cmp(iface->functions.get[i], name))
      return i;
  }
  return -1;
}

void add_impl(Parser* p, Impl impl, size_t iface_idx) {
  Interface* iface = p->interfaces.get[iface_idx];
  for (size_t i = 0; i < iface->impls.n; i++) {
    if (span_cmp(impl.name, iface->impls.get[i]->name))
      throw_diag(&p->engine, iface->name, MM_ERR_IMPL_ALREADY_EXISTS, impl.name, iface->name);
  }
  Impl* mem = vmarena_alloc(p->arena, sizeof(Impl));
  *mem = impl;
  vec_push(p->interfaces.get[iface_idx]->impls, mem);
}

void parse_impl(Parser* p) {
  Span name = get_tok(p);
  expect(p, "as");
  Span iface_name = get_tok(p);

  ssize_t iface_idx = find_iface_idx(p, iface_name);
  if (iface_idx < 0)
    throw_diag(&p->engine, iface_name, MM_ERR_INTERFACE_DOESNT_EXIST, iface_name);

  expect(p, "{");

  ImplKVPair header_pair;
  parse_pair(p, &header_pair);

  if (!span_str_cmp(header_pair.key, "$header")) {
    throw_diag(&p->engine, header_pair.key, MM_ERR_UNEXPECTED_TOK,
               "`$header = none` or `$header = \"xxxxx.h\"`", header_pair.key);
  }
  if (span_str_cmp(header_pair.val, "none")) {
    header_pair.val.len = 0;
  } else if (header_pair.val.str[0] != '"' || header_pair.val.str[header_pair.val.len - 1] != '"') {
    throw_diag(&p->engine, header_pair.val, MM_ERR_HEADER_FILE_NOT_IN_DOUBLE_QUOTES,
               header_pair.val);
  }

  Impl impl = {0};
  impl.header = header_pair.val;
  impl.name = name;

  Interface* iface = p->interfaces.get[iface_idx];
  vec_resize(impl.pairs, iface->functions.n);
  memset(impl.pairs.get, 0, sizeof(ImplKVPair) * iface->functions.n);

  while (true) {
    skip_unwanted(&p->file);
    if (peekch(&p->file) == '}') {
      nextch(&p->file);
      break;
    }
    ImplKVPair pair;
    parse_pair(p, &pair);

    iface = p->interfaces.get[iface_idx];
    int id = find_fn_id(p, iface, pair.key);
    if (id < 0)
      throw_diag(&p->engine, pair.key, MM_ERR_FN_NOT_DEFINED_BUT_REFERENCED, pair.key, iface->name,
                 impl.name);

    impl.pairs.get[id] = pair;
    impl.pairs.n++;
  }

  add_impl(p, impl, iface_idx);
}

void add_export_cli(Parser* p, bstr iface_name, bstr impl_name) {
  int dup_id = -1;
  Span impl_span = str_to_span(impl_name);
  Span iface_span = str_to_span(iface_name);

  for (int i = 0; i < p->exports.n; i++) {
    ExportCmd cmd = p->exports.get[i];
    if (span_str_cmp(cmd.iface->name, iface_name)) {
      print_diag(&p->engine, NULL_SPAN, NOTE_OVERRIDING_EXPORT_CLI, cmd.iface->name, cmd.impl->name,
                 iface_span, impl_span);
      dup_id = i;
    }
  }

  Interface* iface = find_iface(p, iface_span);
  if (iface == NULL)
    throw_diag(&p->engine, NULL_SPAN, MM_ERR_INTERFACE_DOESNT_EXIST, iface_span);

  Impl* impl = find_impl(p, iface, impl_span);
  if (impl == NULL)
    throw_diag(&p->engine, NULL_SPAN, MM_ERR_IMPL_NOT_DEFINED, impl_span, iface_span);

  ExportCmd export = {.iface = iface, .impl = impl};
  if (dup_id < 0)
    vec_push(p->exports, export);
  else
    p->exports.get[dup_id] = export;
}

void add_export_conf(Parser* p, Span iface_name, Span impl_name) {
  int dup_id = -1;
  for (int i = 0; i < p->exports.n; i++) {
    ExportCmd cmd = p->exports.get[i];
    if (span_cmp(cmd.iface->name, iface_name)) {
      print_diag(&p->engine, impl_name, NOTE_OVERRIDING_EXPORT_CONF, cmd.iface->name,
                 cmd.impl->name, iface_name, impl_name);
      dup_id = i;
    }
  }

  Interface* iface = find_iface(p, iface_name);
  if (iface == NULL)
    throw_diag(&p->engine, iface_name, MM_ERR_INTERFACE_DOESNT_EXIST, iface_name);

  Impl* impl = find_impl(p, iface, impl_name);
  if (impl == NULL)
    throw_diag(&p->engine, impl_name, MM_ERR_IMPL_NOT_DEFINED, impl_name, iface_name);

  ExportCmd export = {.iface = iface, .impl = impl};
  if (dup_id < 0)
    vec_push(p->exports, export);
  else
    p->exports.get[dup_id] = export;
}

void parse_export(Parser* p) {
  Span iface_name = get_tok(p);
  expect(p, "as");
  Span impl_name = get_tok(p);
  add_export_conf(p, iface_name, impl_name);
}

void parse_keyw(Parser* p, Span keyw) {
#define X(str, fn)                                                                                 \
  if (span_str_cmp(keyw, str)) {                                                                   \
    fn(p);                                                                                         \
    return;                                                                                        \
  }
  DISPATCH_TABLE(X)
#undef X
  throw_diag(&p->engine, keyw, MM_ERR_UNEXPECTED_KEYW, keyw);
}

void parse_conf(Parser* p) {
  while (true) {
    if (peekch(&p->file) == EOF)
      break;
    Span keyw = get_tok(p);
    if (keyw.len == 0)
      break;
    parse_keyw(p, keyw);
  }
}

void read_conf(Parser* p, bstr confpath, ExportOverrideVec* export_override, VMEMArena* arena,
               jmp_buf* onerror) {
  *p = (Parser){0};
  p->engine = new_engine(mm_diaginfos, __mm_diagtype_len, onerror);
  p->arena = arena;
  ScannerRes res = read_file(&p->file, arena, confpath);
  if (res != SE_OK)
    throw_diag(&p->engine, NULL_SPAN, MM_ERR_CANT_OPEN_FILE, confpath);
  parse_conf(p);

  for (size_t i = 0; i < export_override->n; i++) {
    ExportOverride ov = export_override->get[i];
    add_export_cli(p, ov.iface, ov.impl);
  }
}

void parser_destroy(Parser* p) {
  vec_destroy(p->exports);
  for (size_t i = 0; i < p->interfaces.n; i++) {
    Interface* iface = p->interfaces.get[i];
    vec_destroy(iface->functions);
    for (size_t j = 0; j < iface->impls.n; j++) {
      vec_destroy(iface->impls.get[j]->pairs);
    }
    vec_destroy(iface->impls);
  }
  vec_destroy(p->interfaces);
}
