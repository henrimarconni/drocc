#ifndef PARSER_H
#define PARSER_H

#include "stringdef.h"
#include "vec.h"
#include "vmem_arena.h"
#include "span.h"
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>


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
  size_t row, col, id, len;
} Position;

typedef struct {
  vec(Interface*) interfaces;
  vec(ExportCmd) exports;
  ostr source;
  Position pos;
  jmp_buf* onerror;
  VMEMArena* arena;
} Parser;

void read_conf(Parser* p, bstr confpath, ExportOverrideVec* export_override, VMEMArena* arena, jmp_buf* onerror);
void parser_destroy(Parser* p);


#endif
