#ifndef PARSER_H
#define PARSER_H

#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include "core/span.h"
#include <setjmp.h>
#include <stdbool.h>


typedef struct {
  bstr impl, iface;
} ExportOverride;

typedef vec(ExportOverride) ExportOverrideVec;

typedef struct {
  Span key, val;
} ImplKVPair;

typedef struct {
  Span name;
  Span header;
  vec(ImplKVPair) pairs;
} Impl;

typedef struct {
  Span name;
  vec(Span) functions;
  vec(Impl*) impls;
  bool is_dynamic;
} Interface;

typedef enum {
  PE_OK,
  PE_ERR
} ParserError;

typedef struct {
  Interface* iface;
  Impl* impl;
} ExportCmd;

typedef struct {
  vec(Interface*) interfaces;
  vec(ExportCmd) exports;
  DiagEngine engine;
  SourceFile file;
  VMEMArena* arena;
} Parser;

void read_conf(Parser* p, bstr confpath, ExportOverrideVec* export_override, VMEMArena* arena, jmp_buf* onerror);
void parser_destroy(Parser* p);


#endif
