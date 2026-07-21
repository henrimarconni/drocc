#include "cparser_utils.h"
#include "diagnostics.h"
#include "parser.h"
#include "scanner.h"
#include "sg_diag.h"
#include "span.h"
#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

bstr toktype_to_str[] = {"Block", "String", "Identifier", "`(`", "`)`", "`:`"};

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

Token tok_block(DiagEngine* engine, TestFile* file, Span span) {
  nextch(&file->source); // skip {
  span.str++;

  int depth = 1;
  bool is_str = false;
  while (depth > 0) {
    int ch = nextch(&file->source);
    skip_c_comments(&file->source, &span);
    if (ch == '"' && !is_str)
      is_str = true;
    if (ch == '"' && is_str)
      is_str = false;
    if (ch == EOF)
      throw_diag(engine, span, ERR_UNEXPECTED_EOF);
    span.len++;
    if (ch == '{')
      depth++;
    else if (ch == '}')
      depth--;
  }
  span.len--; // dont include final }
  return (Token){TOK_BLOCK, span};
}

#define is_id_start(ch) (isalpha((ch)) || (ch) == '_' || (ch) == '$')
#define is_id_body(ch) (isalnum((ch)) || (ch) == '_')

Token tok_id(TestFile* file, Span span) {
  span.len = 1;
  nextch(&file->source); // first character is already included
  int ch = peekch(&file->source);
  while (is_id_body(ch)) {
    nextch(&file->source);
    ch = peekch(&file->source);
    span.len++;
  }
  return (Token){TOK_ID, span};
}

Token tok_str(TestFile* file, Span span) { return (Token){TOK_STR, parse_cstr(&file->source)}; }

Token tok_simple(TokenType type, int ch, Span span) {
  span.len = 1;
  return (Token){type, span};
}

// \t\t\t$
//       ^ (after skip_unwanted)
//        ^ (after nextch)
Token get_tok(DiagEngine* engine, TestFile* file) {
  skip_unwanted(&file->source);
  Span span = span_from_file(&file->source);
  int ch = peekch(&file->source);

  switch (ch) {
  case '{':
    return tok_block(engine, file, span);
  case '"':
    return tok_str(file, span);
  case ':':
    return tok_simple(TOK_COLON, nextch(&file->source), span);
  case '(':
    return tok_simple(TOK_LPAREN, nextch(&file->source), span);
  case ')':
    return tok_simple(TOK_RPAREN, nextch(&file->source), span);
  }
  if (is_id_start(ch))
    return tok_id(file, span);
  throw_diag(engine, span, ERR_UNEXPECTED_CHAR, span);
}

Token expect_tok(DiagEngine* engine, TestFile* file, TokenType type) {
  Token tok = get_tok(engine, file);
  if (tok.type != type)
    throw_diag(engine, tok.span, ERR_UNEXPECTED_TOK, toktype_to_str[type], tok.span);
  return tok;
}

void parse_cblock(DiagEngine* engine, TestFile* file) {
  CodegenNode node = {};
  Token block = get_tok(engine, file);
  node.c_code = block.span;
  node.type = CG_CBLOCK;
  vec_push(file->nodes, node);
}

// Syntax: $test "description" : TestGroup(test_name) {...}
void parse_test(DiagEngine* engine, TestFile* file) {
  CodegenNode node = {};
  node.type = CG_TEST;

  Token desc = expect_tok(engine, file, TOK_STR);
  expect_tok(engine, file, TOK_COLON);
  Token group = expect_tok(engine, file, TOK_ID);
  expect_tok(engine, file, TOK_LPAREN);
  Token name = expect_tok(engine, file, TOK_ID);
  expect_tok(engine, file, TOK_RPAREN);
  Token body = expect_tok(engine, file, TOK_BLOCK);

  node.test.desc = desc.span;
  node.test.group = group.span;
  node.test.name = name.span;
  node.test.body = body.span;

  vec_push(file->nodes, node);
}

void parse_keyw(DiagEngine* engine, TestFile* file, Token keyw) {
  if (span_str_cmp(keyw.span, "$c"))
    parse_cblock(engine, file);
  else if (span_str_cmp(keyw.span, "$test"))
    parse_test(engine, file);
  else
    throw_diag(engine, keyw.span, ERR_UNEXPECTED_KEYW, keyw.span);
}

void print_codegen_node(CodegenNode node) {
  printf("Node(%d): ", node.type);
  if (node.type == CG_CBLOCK)
    printf("\n$c {\n%.*s}\n", (int)node.c_code.len, node.c_code.str);
  else if (node.type == CG_TEST) {
    printf("$test \"%.*s\" : %.*s(%.*s)\n", (int)node.test.desc.len, node.test.desc.str,
           (int)node.test.group.len, node.test.group.str, (int)node.test.name.len,
           node.test.name.str);
    printf("\n{\n%.*s}\n", (int)node.test.body.len, node.test.body.str);
  }
}

void parse_file(ParserState* state, TestFile* file) {
  while (true) {
    skip_unwanted(&file->source);
    if (peekch(&file->source) == EOF)
      break;
    Token token = get_tok(&state->engine, file);
    parse_keyw(&state->engine, file, token);
  }
}

void parse_files(ParserState* state, InputFiles files, jmp_buf* onerror) {
  state->engine = new_engine(sg_diaginfos, __sg_diagtype_len, onerror);

  for (size_t i = 0; i < files.n; i++) {
    TestFile file = {};
    ScannerRes res = read_file(&file.source, state->arena, files.get[i]);
    if (res != SE_OK)
      throw_diag(&state->engine, NULL_SPAN, ERR_CANT_OPEN_FILE, files.get[i]);
    parse_file(state, &file);
    vec_push(state->files, file);
  }
}
