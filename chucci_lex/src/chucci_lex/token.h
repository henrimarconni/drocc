#ifndef TOKEN_H
#define TOKEN_H

#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vec.h"
#include <assert.h>
#include <stdbool.h>

extern const bool is_op_table[256];
extern const bool is_sep_table[256];
extern const char* tok_to_str[];

#define is_op(ch) (is_op_table[ch])
#define is_sep(ch) (is_sep_table[ch])
#define cursor_from_token(tok)                                                                     \
  ((Cursor){.id = (tok).pos.id, .line = (tok).pos.line, .col = (tok).pos.col, .source = (tok).file})

#define OPERATORS(X)                                                                               \
  X(OP_SHL_EQ, "<<=", '<')                                                                         \
  X(OP_SHR_EQ, ">>=", '>')                                                                         \
  X(OP_ELLIPSIS, "...", '.')                                                                       \
  X(OP_INC, "++", '+')                                                                             \
  X(OP_DEC, "--", '-')                                                                             \
  X(OP_EQ, "==", '=')                                                                              \
  X(OP_NEQ, "!=", '!')                                                                             \
  X(OP_LE, "<=", '<')                                                                              \
  X(OP_GE, ">=", '>')                                                                              \
  X(OP_AND, "&&", '&')                                                                             \
  X(OP_OR, "||", '|')                                                                              \
  X(OP_SHL, "<<", '<')                                                                             \
  X(OP_SHR, ">>", '>')                                                                             \
  X(OP_TOKEN_PASTE, "##", '#')                                                                     \
  X(OP_ADD_EQ, "+=", '+')                                                                          \
  X(OP_SUB_EQ, "-=", '-')                                                                          \
  X(OP_MUL_EQ, "*=", '*')                                                                          \
  X(OP_DIV_EQ, "/=", '/')                                                                          \
  X(OP_MOD_EQ, "%=", '%')                                                                          \
  X(OP_AND_EQ, "&=", '&')                                                                          \
  X(OP_OR_EQ, "|=", '|')                                                                           \
  X(OP_XOR_EQ, "^=", '^')                                                                          \
  X(OP_ARROW, "->", '-')                                                                           \
  X(OP_ADD, "+", '+')                                                                              \
  X(OP_SUB, "-", '-')                                                                              \
  X(OP_MUL, "*", '*')                                                                              \
  X(OP_DIV, "/", '/')                                                                              \
  X(OP_MOD, "%", '%')                                                                              \
  X(OP_ASSIGN, "=", '=')                                                                           \
  X(OP_LT, "<", '<')                                                                               \
  X(OP_GT, ">", '>')                                                                               \
  X(OP_NOT, "!", '!')                                                                              \
  X(OP_BIT_AND, "&", '&')                                                                          \
  X(OP_BIT_OR, "|", '|')                                                                           \
  X(OP_BIT_XOR, "^", '^')                                                                          \
  X(OP_BIT_NOT, "~", '~')                                                                          \
  X(OP_QUESTION, "?", '?')                                                                         \
  X(OP_COLON, ":", ':')                                                                            \
  X(OP_PREPROCESS, "#", '#')                                                                       \
  X(OP_DOT, ".", '.')

#define SEPARATORS(X)                                                                              \
  X(SEP_LPAREN, "(", '(')                                                                          \
  X(SEP_RPAREN, ")", ')')                                                                          \
  X(SEP_LCURLY, "{", '{')                                                                          \
  X(SEP_RCURLY, "}", '}')                                                                          \
  X(SEP_LSQ, "[", '[')                                                                             \
  X(SEP_RSQ, "]", ']')                                                                             \
  X(SEP_COMMA, ",", ',')                                                                           \
  X(SEP_NEWLINE, "\n", '\n')                                                                       \
  X(SEP_SEMI, ";", ';')

#define KEYWORDS(X)                                                                                \
  X(KW_IF, "if")                                                                                   \
  X(KW_ELSE, "else")                                                                               \
  X(KW_WHILE, "while")                                                                             \
  X(KW_FOR, "for")                                                                                 \
  X(KW_RETURN, "return")                                                                           \
  X(KW_BREAK, "break")                                                                             \
  X(KW_CONTINUE, "continue")                                                                       \
  X(KW_STRUCT, "struct")                                                                           \
  X(KW_ENUM, "enum")                                                                               \
  X(KW_TYPEDEF, "typedef")                                                                         \
  X(KW_CONST, "const")                                                                             \
  X(KW_STATIC, "static")                                                                           \
  X(KW_VOID, "void")                                                                               \
  X(KW_INT, "int")                                                                                 \
  X(KW_FLOAT, "float")                                                                             \
  X(KW_CHAR, "char")                                                                               \
  X(KW_SIZEOF, "sizeof")

#define PREPROCESSOR_CMD(X)                                                                        \
  X(PP_DEFINE, "define")                                                                           \
  X(PP_UNDEF, "undef")                                                                             \
  X(PP_IFDEF, "ifdef")                                                                             \
  X(PP_IFNDEF, "ifndef")                                                                           \
  X(PP_ELIF, "elif")                                                                               \
  X(PP_ENDIF, "endif")                                                                             \
  X(PP_ERROR, "error")                                                                             \
  X(PP_LINE, "line")                                                                               \
  X(PP_PRAGMA, "pragma")

#define is_tok_op_or_sep(token) (token.kind < TOK_EOF && token.kind > KW_SIZEOF)

typedef enum {
#define X(a, b) a,
  PREPROCESSOR_CMD(X)
#undef X
      __preprocessor_cmd_len
} PPCmd;

typedef enum TokenKind {

// X-macro generated kinds start
#define X(a, b) a,
  KEYWORDS(X)
#undef X
#define X(a, b, c) a,
      OPERATORS(X)
#undef X
#define X(a, b, c) a,
          SEPARATORS(X)
#undef X
// end

  TOK_EOF,   //< End-of-File
  TOK_IDENT, //< Variable/Function/... names
  TOK_VAL,   //< String literals or numerical values
  TOK_PP,    //< Preprocessor commands
  __token_kind_count,
} TokenKind;

typedef struct {
  Span span;
  TokenKind kind;
  /**
    If token is an identifier, we intern it
    and store the identifier InternID.
    THIS IS AN OPTIONAL value
  */
  InternID ident;
} Token;

#define NULL_TOKEN (Token){NULL_SPAN, TOK_EOF, 0}
typedef vec(Token) TokenVec;

Token new_tok_ident(Span span, InternID ident);
Token new_tok_val(Span span);
Token new_tok_simple(Span, TokenKind kind);

void print_token(SourceManager* sman, Token* token);
void print_token_pretty(SourceManager* sman, Token* token);
#endif
