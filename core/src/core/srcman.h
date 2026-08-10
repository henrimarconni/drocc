#ifndef SRCMAN_H_
#define SRCMAN_H_

#include "core/stringdef.h"
#include "core/vec.h"
#include "core/vmem_arena.h"
#include <stdint.h>

#define INVALID_SRC_ID 65535

#define NULL_SPAN (Span){ .offset = 0, .len = 0, .srcid = INVALID_SRC_ID }
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RESET "\x1b[0m"


typedef struct Span {
  uint64_t offset : 32;
  uint64_t len : 16;
  uint16_t srcid : 16;
} Span;

typedef uint16_t SrcID;

/// Source file
typedef struct {
  /// Offsets to the beginning of each line
  vec(uint32_t) offsets;
  size_t len;
  bstr name;
  bstr contents;
} SMSource;

typedef struct {
  vec(SMSource) sources;
} SourceManager;

typedef struct SrcScanner {
  size_t id;
  SourceManager* sman;
  SrcID srcid;
} SrcScanner;

typedef struct {
  uint16_t row, col;
  uint32_t id;
  StringView sv;
  SMSource* file;
} SMSpanInfo;


SourceManager sman_new();
void sman_free(SourceManager* sman);
/**
  Open a file and allocate the contents in arena
  @return INVALID_SRC_ID if file cannot be opened, else the valid SrcID
*/
SrcID sman_open(SourceManager* man, bstr const name, VMEMArena* arena);

/**
  Consider a string as a file
  @note This doesnt allocate memory, so the contents you provide must live till the SourceManager
*/
SrcID sman_str(SourceManager* man, bstr const name, bstr const contents, size_t len);

/// Get information about the span
SMSpanInfo sman_info(SourceManager* sman, Span span);



#endif
