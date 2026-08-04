#ifndef PARSER_H
#define PARSER_H

#include "core/diagnostics.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <setjmp.h>
#include <stdbool.h>


typedef struct {
  bstr impl, iface;
} ExportOverride;

typedef struct MMPair MMPair;
typedef struct MMToken MMToken;
typedef struct Interface Interface;
typedef struct Impl Impl ;

typedef vec(ExportOverride) ExportOverrideVec;

typedef struct {
  Interface* iface;
  Impl* impl;
} ExportCmd;

typedef struct {
  VMEMArena* arena;
  vec(Interface*) interfaces;
  vec(ExportCmd) exports;
  DiagEngine engine;
  SourceManager* sman;
  SrcScanner scanner;
} MMParser;


void read_conf(MMParser* p, SourceManager* sman, VMEMArena* arena, bstr file, ExportOverrideVec* export_override, jmp_buf* onerror);
void parser_destroy(MMParser* p);


#endif
