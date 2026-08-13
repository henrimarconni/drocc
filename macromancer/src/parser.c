#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include "core/strutils.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "macromancer/mm_diag.h"
#include "macromancer/mmtok.h"
#include "macromancer/parser.h"
#include <assert.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_ID_LEN 128

#define DISPATCH_TABLE(X)                                                                          \
  X("$impl", parse_impl)                                                                           \
  X("$interface", parse_interface)                                                                 \
  X("$export", parse_export)

void add_interface(MMParser* p, Interface interface) {
  for (size_t i = 0; i < p->interfaces.n; i++) {
    if (sv_cmp(interface.name.sv, p->interfaces.get[i]->name.sv))
      throw_diag(&p->engine, interface.name.span, MM_ERR_INTERFACE_ALREADY_EXISTS, interface.name);
  }
  Interface* mem = vmarena_alloc(p->arena, sizeof(Interface));
  *mem = interface;
  vec_push(p->interfaces, mem);
}

Impl* find_impl(MMParser* p, Interface* iface, StringView name) {
  for (size_t i = 0; i < iface->impls.n; i++) {
    Impl* impl = iface->impls.get[i];
    if (sv_cmp(impl->name.sv, name))
      return impl;
  }
  return NULL;
}

int find_iface_idx(MMParser* p, StringView name) {
  for (int i = 0; i < p->interfaces.n; i++) {
    if (sv_cmp(p->interfaces.get[i]->name.sv, name))
      return i;
  }
  return -1;
}

Interface* find_iface(MMParser* p, StringView name) {
  int idx = find_iface_idx(p, name);
  if (idx < 0)
    return NULL;
  return p->interfaces.get[idx];
}

int find_fn_id(MMParser* p, Interface* iface, Span name) {
  for (size_t i = 0; i < iface->functions.n; i++) {
    if (span_cmp(p->sman, iface->functions.get[i].span, name))
      return i;
  }
  return -1;
}

void add_impl(MMParser* p, Impl impl, size_t iface_idx) {
  Interface* iface = p->interfaces.get[iface_idx];
  for (size_t i = 0; i < iface->impls.n; i++) {
    if (sv_cmp(impl.name.sv, iface->impls.get[i]->name.sv)) {
      throw_diag(&p->engine, iface->name.span, MM_ERR_IMPL_ALREADY_EXISTS, impl.name, iface->name);
    }
  }
  Impl* mem = vmarena_alloc(p->arena, sizeof(Impl));
  *mem = impl;
  vec_push(p->interfaces.get[iface_idx]->impls, mem);
}

void parse_impl(MMParser* p) {
  Impl impl = {0};

  impl.name = expect_mmtok(p, MMT_IDENT);
  expect_mmtok(p, MMT_AS);
  MMToken iface_tok = expect_mmtok(p, MMT_IDENT);

  expect_mmtok(p, MMT_LCURLY);
  impl.header = expect_mmtok(p, MMT_HEADERPAIR);

  while (true) {
    MMPair pair = {0};
    MMToken tok = get_mmtok(p);
    if (tok.type == MMT_RCURLY)
      break;
    if (tok.type != MMT_IDENT) {
      throw_diag(
          &p->engine,
          tok.span,
          MM_ERR_UNEXPECTED_TOK,
          mmtok_type_to_str[tok.type],
          mmtok_type_to_str[MMT_IDENT]);
    }

    pair.key = tok.sv;
    expect_mmtok(p, MMT_EQ);
    pair.val = expect_mmtok(p, MMT_IDENT).sv;

    vec_push(impl.pairs, pair);
  }

  int idx = find_iface_idx(p, iface_tok.sv);
  if (idx < 0)
    throw_diag(&p->engine, iface_tok.span, MM_ERR_INTERFACE_DOESNT_EXIST, iface_tok.sv);
  add_impl(p, impl, idx);
}

void parse_interface(MMParser* p) {
  Interface iface = {0};

  iface.name = expect_mmtok(p, MMT_IDENT);
  MMToken as = expect_mmtok(p, MMT_AS);
  MMToken type = expect_mmtok(p, MMT_IDENT);

  // Dynamic or Static
  if (strncmp(type.sv.str, "Dynamic", strlen("Dynamic")) == 0)
    iface.is_dynamic = true;
  else if (strncmp(type.sv.str, "Static", strlen("Static")) == 0)
    iface.is_dynamic = false;
  else
    throw_diag(&p->engine, type.span, MM_ERR_UNEXPECTED_IFACE_TYPE, type.sv);

  // Function list start
  expect_mmtok(p, MMT_LCURLY);

  while (true) {
    MMToken tok = get_mmtok(p);
    if (tok.type == MMT_RCURLY)
      break;
    if (tok.type != MMT_IDENT) {
      throw_diag(
          &p->engine,
          tok.span,
          MM_ERR_UNEXPECTED_TOK,
          mmtok_type_to_str[tok.type],
          mmtok_type_to_str[MMT_IDENT]);
    }
    vec_push(iface.functions, tok);
  }

  add_interface(p, iface);
}

void add_export(MMParser* p, ExportCmd cmd) {
  for (size_t i = 0; i < p->exports.n; i++) {
    ExportCmd* cmd2 = &p->exports.get[i];
    if (cmd.iface == cmd2->iface) {
      print_diag(
          &p->engine,
          NULL_SPAN,
          MM_NOTE_OVERRIDING_EXPORT,
          cmd2->iface->name.sv,
          cmd2->impl->name.sv,
          cmd.iface->name.sv,
          cmd.impl->name.sv);
      *cmd2 = cmd;
      return;
    }
  }

  vec_push(p->exports, cmd);
}

void parse_export(MMParser* p) {
  ExportCmd cmd = {0};
  MMToken iface_tok = expect_mmtok(p, MMT_IDENT);
  cmd.iface = find_iface(p, iface_tok.sv);

  expect_mmtok(p, MMT_AS);

  MMToken impl_tok = expect_mmtok(p, MMT_IDENT);
  cmd.impl = find_impl(p, cmd.iface, impl_tok.sv);

  add_export(p, cmd);
}

void parse_keyw(MMParser* p, MMToken keyw) {
#define X(kstr, fn)                                                                                \
  if (strncmp(kstr, keyw.sv.str, keyw.sv.len) == 0) {                                              \
    fn(p);                                                                                         \
    return;                                                                                        \
  }
  DISPATCH_TABLE(X)
#undef X
  throw_diag(&p->engine, keyw.span, MM_ERR_UNEXPECTED_KEYW, keyw.sv);
}

void parse_conf(MMParser* p) {
  while (true) {
    skip_unwanted(&p->scanner);
    if (peekch(&p->scanner) == EOF)
      break;
    MMToken keyw = expect_mmtok(p, MMT_KEYW);
    parse_keyw(p, keyw);
  }
}

void read_conf(
    MMParser* p,
    SourceManager* sman,
    VMEMArena* arena,
    bstr file,
    ExportOverrideVec* export_override,
    jmp_buf* onerror) {
  *p = (MMParser){0};

  p->arena = arena;
  p->sman = sman;
  p->engine = new_engine(mm_diaginfos, __mm_diagtype_len, p->sman, onerror);

  if (!sman_open(&p->scanner, p->sman, file))
    throw_diag(&p->engine, NULL_SPAN, MM_ERR_CANT_OPEN_FILE, file);

  parse_conf(p);

  for (size_t i = 0; i < export_override->n; i++) {
    ExportOverride ov = export_override->get[i];
    ExportCmd cmd = {0};
    cmd.iface = find_iface(p, strview(ov.iface));
    if (!cmd.iface)
      throw_diag(&p->engine, NULL_SPAN, MM_ERR_INTERFACE_DOESNT_EXIST, ov.iface);

    cmd.impl = find_impl(p, cmd.iface, strview(ov.impl));
    if (!cmd.impl)
      throw_diag(&p->engine, NULL_SPAN, MM_ERR_IMPL_NOT_DEFINED, ov.impl);

    add_export(p, cmd);
  }
}

void parser_destroy(MMParser* p) {
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
