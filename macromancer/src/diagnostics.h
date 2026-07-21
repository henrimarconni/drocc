#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "parser.h"
#include <stdio.h> // IWYU pragma: keep


#define ERROR_STR ANSI_RED "Error: " ANSI_RESET
#define NOTE_STR ANSI_YELLOW "Note: " ANSI_RESET

#define ERRORS(X) \
X(ERR_UNEXPECTED_TOK, "Expected `%s` found `%span`")\
X(ERR_UNEXPECTED_KEYW, "Unexpected keyword %span, expected one of `$interface`, `$export`, `$impl`")\
X(ERR_CANT_OPEN_FILE, "Cannot open file: %s")\
X(ERR_INTERFACE_DOESNT_EXIST, "Interface %span doesn't exist")\
X(ERR_HEADER_FILE_NOT_IN_DOUBLE_QUOTES, "Header file must be in double quotes and shouldn't have spaces: found %span")\
X(ERR_FN_NOT_DEFINED_BUT_REFERENCED, "Function %span is not defined in interface %span but is referenced in implementation %span")\
X(ERR_IMPL_NOT_DEFINED, "No implementation %span found for interface %span")\
X(ERR_IMPL_ALREADY_EXISTS, "Implementation named %span already exists in interface %span")\
X(ERR_INTERFACE_ALREADY_EXISTS, "Interface %span already exists")\
X(ERR_INVALID_IDENTIFIER, "Expected a identifier, found %span")\
X(ERR_INVALID_STRING, "Expected a string, found %span")

#define NOTES(X)\
X(NOTE_OVERRIDING_EXPORT_CLI, "Overriding `export %span as %span` with `export %span as %span` from command-line argument `--export`")\
X(NOTE_OVERRIDING_EXPORT_CONF, "Overriding `export %span as %span` with `export %span as %span` from configuration")


#define FORMAT_SPECS(X) \
X("%span", print_span, Span)\
X("%s", print_str, bstr)\
X("%c", putchar, int)

typedef enum {
#define X(a, _) a,
  NOTES(X)
#undef X
__note_type_len
} NoteType;

typedef enum {
#define X(a, _) a,
  ERRORS(X)
#undef X
__error_type_len
} ErrorType;


#define throw_error(p, span, type, ...)\
do {\
  printf("In %s:%d %s():\n", __FILE__, __LINE__, __func__);\
  _throw_error((p), (span), (type) __VA_OPT__(,) __VA_ARGS__);\
} while (0)

#define add_note(p, type, ...)\
do {\
  printf("In %s:%d %s():\n", __FILE__, __LINE__, __func__);\
  _add_note((p), (type), __VA_ARGS__);\
} while (0)


void _throw_error(Parser* p, Span err_span, ErrorType type, ...);
void _add_note(Parser* p, NoteType type, ...);

#endif
