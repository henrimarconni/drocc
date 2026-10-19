/**
  @file
  Clean and lightweight diagnostics engine
*/

#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

#include "core/srcman.h"
#include "core/diag_core.h"
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h> // IWYU pragma: keep

typedef struct {
  const DiagInfo* infos;
  size_t info_len;
  jmp_buf* onerror;
  SourceManager* sman;
} DiagEngine;

/**
  Print the error with highlighted span and then longjmp to the DiagEngine.onerror
  @param span If there is no span related to the error, pass `NULL_SPAN`
  @note You should not use it directly, instead, use the throw_diag macros for a more informative
  diagnostic
  @see _print_diag
  @see throw_diag
*/
[[noreturn]]
void _throw_diag(DiagEngine* eng, Span span, int diag_type, ...);

/**
  Print the error with highlighted span
  @param span If there is no span related to the error, pass `NULL_SPAN`
  @note You should not use it directly, instead, use the throw_diag macros for a more informative
diagnostic
  @see print_diag
  @see _throw_diag
  @see throw_diag
_*/
void _print_diag(DiagEngine* eng, Span span, int diag_type, ...);

/// Populates DiagEngine with the given parameters
DiagEngine
new_engine(const DiagInfo* infos, size_t info_len, SourceManager* sman, jmp_buf* onerror);

/**
  Throws a diagnostic exception containing additional caller information.
  @see print_diag
  @see _throw_diag
  @see throw_diag
*/
#define throw_diag(eng, span, type, ...)                                                           \
  do {                                                                                             \
    printf("In %s:%d %s():\n", __FILE__, __LINE__, __func__);                                      \
    _throw_diag((eng), (span), (type)__VA_OPT__(, ) __VA_ARGS__);                                  \
  } while (0)

/**
  Print a diagnostic exception containing additional caller information.
  @see print_diag
  @see _throw_diag
  @see throw_diag
*/
#define print_diag(eng, type, ...)                                                                 \
  do {                                                                                             \
    printf("In %s:%d %s():\n", __FILE__, __LINE__, __func__);                                      \
    _print_diag((eng), (type), __VA_ARGS__);                                                       \
  } while (0)

#endif
