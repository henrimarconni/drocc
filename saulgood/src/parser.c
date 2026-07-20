#include "parser.h"
#include "utils.h"

void parse_file(ParserState* state, bstr filename) {
  TestFile file = {};
  file.source = read_file(state->arena, filename);

  while (true) {
  }
}
