#include "parser.h"
#include "scanner.h"
#include "span.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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

void skip_c_comments(SourceFile* file, Span* span) {
  while (peekch(file) == '/') {
    if (peeknextch(file) == '/') {
      char ch;
      while ((ch = peekch(file)) != '\n') {
        ch = nextch(file);
        assert(ch != EOF);
        span->len++;
      }
    } else if (peeknextch(file) == '*') {
      nextch(file); // skip /
      span->len++;
      nextch(file); // skip *
      span->len++;
      char ch = peekch(file);
      while (peekch(file) != '*' || peeknextch(file) != '/') {
        nextch(file);
        assert(ch != EOF);
        span->len++;
      }
      nextch(file); // skip *
      span->len++;
      nextch(file); // skip /
      span->len++;
    } else
      break;
  }
}

Token tok_block(TestFile* file, Span span) {
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
    assert(ch != EOF);
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

Token tok_str(TestFile* file, Span span) {
  nextch(&file->source);
  span.str++; // skip "
  int ch = peekch(&file->source);
  while (ch != '"') {
    assert(ch != EOF);
    nextch(&file->source);
    ch = peekch(&file->source);
    span.len++;
  }
  nextch(&file->source); // skip ending "
  return (Token){TOK_STR, span};
}

Token tok_simple(TokenType type, int ch, Span span) {
  span.len = 1;
  return (Token){type, span};
}

// \t\t\t$
//       ^ (after skip_unwanted)
//        ^ (after nextch)
Token get_tok(TestFile* file) {
  skip_unwanted(&file->source);
  Span span = {file->source.pos.row, file->source.pos.col, 0,
               &file->source.contents[file->source.pos.id]};
  int ch = peekch(&file->source);

  switch (ch) {
  case '{':
    return tok_block(file, span);
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
  assert(false && "TODO: diagnostics");
}

Token expect_tok(TestFile* file, TokenType type) {
  Token tok = get_tok(file);
  // printf("asserting: %d == %d [%.*s]\n", type, tok.type, (int)tok.span.len, tok.span.str);
  assert(tok.type == type);
  return tok;
}

void parse_cblock(TestFile* file) {
  CodegenNode node = {};
  Token block = get_tok(file);
  node.c_code = block.span;
  node.type = CG_CBLOCK;
  vec_push(file->nodes, node);
}

// Syntax: $test "description" : TestGroup(test_name) {...}
void parse_test(TestFile* file) {
  CodegenNode node = {};
  node.type = CG_TEST;

  Token desc = expect_tok(file, TOK_STR);
  expect_tok(file, TOK_COLON);
  Token group = expect_tok(file, TOK_ID);
  expect_tok(file, TOK_LPAREN);
  Token name = expect_tok(file, TOK_ID);
  expect_tok(file, TOK_RPAREN);
  Token body = expect_tok(file, TOK_BLOCK);

  node.test.desc = desc.span;
  node.test.group = group.span;
  node.test.name = name.span;
  node.test.body = body.span;

  vec_push(file->nodes, node);
}

void parse_keyw(TestFile* file, Token keyw) {
  if (span_str_cmp(keyw.span, "$c"))
    parse_cblock(file);
  else if (span_str_cmp(keyw.span, "$test"))
    parse_test(file);
  else
    assert(false);
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
    Token token = get_tok(file);
    parse_keyw(file, token);
  }
}

void parse_files(ParserState* state, InputFiles files) {
  for (size_t i = 0; i < files.n; i++) {
    TestFile file = {};
    file.source.name = files.get[i];
    file.source.contents = read_file(state->arena, file.source.name);
    file.source.pos.len = strlen(file.source.contents);
    parse_file(state, &file);
    vec_push(state->files, file);
  }
}
