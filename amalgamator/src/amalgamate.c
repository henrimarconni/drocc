#include "amalgamate.h"
#include "cparser_utils.h"
#include "scanner.h"
#include "span.h"
#include "stringbuilder.h"
#include "utils.h"
#include "vmem_arena.h"

typedef struct {
  StringBuilder output;
  SourceFile file;
  VMEMArena* arena;
} Amalgamator;

void preprocessor_cmd(Amalgamator* a);

void skip_unwanted(SourceFile* file) {
  int last_id;
  do {
    last_id = file->pos.id;
    Span _span = {};
    skip_c_comments(file, &_span);
    skip_space(file);
  } while (file->pos.id != last_id && peekch(file) != EOF);
}

void process_file(Amalgamator* a) {
  char ch;
  while ((ch = peekch(&a->file)) != EOF) {
    skip_unwanted(&a->file);
    if (ch == '#')
      preprocessor_cmd(a);
  }
}

void process_include(Amalgamator* a) { Span include_file = parse_cstr(&a->file); }

void preprocessor_cmd(Amalgamator* a) {
  skip_unwanted(&a->file);
  Span span = span_from_file(&a->file);
  char ch;
  while ((ch = peekch(&a->file)) != EOF) {
    if (span.len > strlen("include"))
      break;
    if (span_str_cmp(span, "include")) {
      skip_space(&a->file);
      if (peekch(&a->file) == '"')
        process_include(a);
      else
        break;
    }
    span.len++;
    nextch(&a->file);
  }
}

StringBuilder amalgamate(VMEMArena* arena, IncludeDirVec idirs, bstr output, bstr input) {
  Amalgamator a = {};
  a.file = read_file(arena, input);
  a.arena = arena;

  return a.output;
}
