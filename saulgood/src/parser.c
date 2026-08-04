#include "core/clexer_utils.h"
#include "core/diagnostics.h"
#include "core/scanner.h"
#include "core/span.h"
#include "core/srcman.h"
#include "core/stringdef.h"
#include "core/vec.h"
#include "saulgood/parser.h"
#include "saulgood/sg_diag.h"
#include <ctype.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

bstr toktype_to_str[] = {"Block", "String", "Identifier", "`(`", "`)`", "`:`"};

void skip_comment(SrcScanner* file) {
  if (peekch(file) == '#') {
    while (peekch(file) != '\n' && peekch(file) != EOF) {
      nextch(file);
    }
  }
}

void skip_unwanted(SrcScanner* file) {
  int last_id;
  do {
    last_id = file->id;
    skip_comment(file);
    skip_space(file);
  } while (file->id != last_id && peekch(file) != EOF);
}

SGToken tok_block(DiagEngine* engine, TestFile* file) {
  nextch(&file->scanner); // skip {
  Span span = span_begin(&file->scanner);

  int depth = 1;
  while (depth > 0) {
    if (skip_c_comments(&file->scanner) < 0)
      throw_diag(engine, span, SG_ERR_UNEXPECTED_EOF);
    else if (peekch(&file->scanner) == '"') {
      if (lex_cstr(&file->scanner) < 0)
        throw_diag(engine, span, SG_ERR_UNEXPECTED_EOF);
    }

    int ch = nextch(&file->scanner);

    if (ch == EOF)
      throw_diag(engine, span, SG_ERR_UNEXPECTED_EOF);
    else if (ch == '{')
      depth++;
    else if (ch == '}')
      depth--;
  }
  span_end(&span, &file->scanner);
  span.len--; // skip last }
  StringView sv = span_sv(file->scanner.sman, span);
  return (SGToken){TOK_BLOCK, sv, span};
}

#define is_id_start(ch) (isalpha((ch)) || (ch) == '_' || (ch) == '$')
#define is_id_body(ch) (isalnum((ch)) || (ch) == '_')

SGToken tok_id(TestFile* file) {
  Span span = span_begin(&file->scanner);
  nextch(&file->scanner); // first character
  int ch = peekch(&file->scanner);
  while (is_id_body(ch)) {
    nextch(&file->scanner);
    ch = peekch(&file->scanner);
  }
  span_end(&span, &file->scanner);
  StringView sv = span_sv(file->scanner.sman, span);
  return (SGToken){TOK_ID, sv, span};
}

SGToken tok_str(DiagEngine* engine, TestFile* file) {
  Span span = span_begin(&file->scanner);
  if (lex_cstr(&file->scanner) < 0)
    throw_diag(engine, span, SG_ERR_UNEXPECTED_EOF);
  span_end(&span, &file->scanner);
  StringView sv = span_sv(file->scanner.sman, span);
  return (SGToken){TOK_STR, sv, span};
}

SGToken tok_simple(TestFile* file, SGTokenType type, int ch, Span span) {
  span.len = 1;
  StringView sv = span_sv(file->scanner.sman, span);
  return (SGToken){type, sv, span};
}

// \t\t\t$
//       ^ (after skip_unwanted)
//        ^ (after nextch)
SGToken get_tok(DiagEngine* engine, TestFile* file) {
  skip_unwanted(&file->scanner);
  Span span = span_begin(&file->scanner);
  int ch = peekch(&file->scanner);

  switch (ch) {
  case '{':
    return tok_block(engine, file);
  case '"':
    return tok_str(engine, file);
  case ':':
    return tok_simple(file, TOK_COLON, nextch(&file->scanner), span);
  case '(':
    return tok_simple(file, TOK_LPAREN, nextch(&file->scanner), span);
  case ')':
    return tok_simple(file, TOK_RPAREN, nextch(&file->scanner), span);
  default:
    if (is_id_start(ch))
      return tok_id(file);
    span_end(&span, &file->scanner);
    throw_diag(engine, span, SG_ERR_UNEXPECTED_CHAR, span);
  }
}

SGToken expect_tok(DiagEngine* engine, TestFile* file, SGTokenType type) {
  SGToken tok = get_tok(engine, file);
  if (tok.type != type)
    throw_diag(engine, tok.span, SG_ERR_UNEXPECTED_TOK, toktype_to_str[type], tok.span);
  return tok;
}

void parse_cblock(DiagEngine* engine, TestFile* file) {
  SGCodegenNode node = {};
  SGToken block = expect_tok(engine, file, TOK_BLOCK);
  node.c_code = block;
  node.type = CG_CBLOCK;
  vec_push(file->nodes, node);
}

// Syntax: $test "description" : TestGroup(test_name) {...}
void parse_test(DiagEngine* engine, TestFile* file) {
  SGCodegenNode node = {};
  node.type = CG_TEST;

  node.test.desc = expect_tok(engine, file, TOK_STR);
  expect_tok(engine, file, TOK_COLON);
  node.test.group = expect_tok(engine, file, TOK_ID);
  expect_tok(engine, file, TOK_LPAREN);
  node.test.name = expect_tok(engine, file, TOK_ID);
  expect_tok(engine, file, TOK_RPAREN);
  node.test.body = expect_tok(engine, file, TOK_BLOCK);

  vec_push(file->nodes, node);
}

void parse_keyw(DiagEngine* engine, TestFile* file, SGToken keyw) {
  if (span_str_cmp(file->scanner.sman, keyw.span, "$c"))
    parse_cblock(engine, file);
  else if (span_str_cmp(file->scanner.sman, keyw.span, "$test"))
    parse_test(engine, file);
  else
    throw_diag(engine, keyw.span, SG_ERR_UNEXPECTED_KEYW, keyw.span);
}

void parse_file(ParserState* state, TestFile* file) {
  while (true) {
    skip_unwanted(&file->scanner);
    if (peekch(&file->scanner) == EOF)
      break;
    SGToken token = get_tok(&state->engine, file);
    parse_keyw(&state->engine, file, token);
  }
}

void parse_files(ParserState* state, SourceManager* sman, InputFiles files, jmp_buf* onerror) {
  state->engine = new_engine(sg_diaginfos, __sg_diagtype_len, sman, onerror);

  for (size_t i = 0; i < files.n; i++) {
    TestFile file = {0};
    SrcID srcid = sman_open(sman, files.get[i], state->arena);
    if (srcid == INVALID_SRC_ID)
      throw_diag(&state->engine, NULL_SPAN, SG_ERR_CANT_OPEN_FILE, files.get[i]);

    file.scanner = scanner_new(sman, srcid);
    parse_file(state, &file);
    vec_push(state->files, file);
  }
}

void parser_free(ParserState* state) {
  for (size_t i = 0; i < state->files.n; i++) {
    TestFile file = state->files.get[i];
    vec_destroy(file.nodes);
  }
  vec_destroy(state->files);
}
