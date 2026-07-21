#ifndef CORE_DIAGNOSTICS_H
#define CORE_DIAGNOSTICS_H

#include "span.h"
#include "stringdef.h"
#include "vec.h"
#include <setjmp.h>
#include <stdio.h> // IWYU pragma: keep
#include <stddef.h>

#define ERROR_STR ANSI_RED "Error: " ANSI_RESET
#define NOTE_STR ANSI_YELLOW "Note: " ANSI_RESET

#define FORMAT_SPECS(X) \
X("%span", print_span, Span)\
X("%s", print_str, bstr)\
X("%c", putchar, int)


typedef enum {
  DL_NOTE,
  DL_ERROR,
} DiagLevel;

typedef struct {
  DiagLevel level;
  bstr msg;
} DiagInfo;

typedef struct Diag Diag;

typedef struct {
  DiagInfo* infos;
  vec(Diag) diags;
  size_t info_len;
  jmp_buf* onerror;
} DiagEngine;

[[noreturn]]
void _throw_diag(DiagEngine* eng, Span span, int diag_type, ...);

void _print_diag(DiagEngine* eng, Span span, int diag_type, ...);
DiagEngine new_engine(DiagInfo* infos, size_t info_len, jmp_buf* onerror);

#define throw_diag(eng, span, type, ...)\
do {\
  printf("In %s:%d %s():\n", __FILE__, __LINE__, __func__);\
  _throw_diag((eng), (span), (type) __VA_OPT__(,) __VA_ARGS__);\
} while (0)

#define print_diag(eng, type, ...)\
do {\
  printf("In %s:%d %s():\n", __FILE__, __LINE__, __func__);\
  _print_diag((eng), (type), __VA_ARGS__);\
} while (0)


#endif
