/**
  @file
  Span implementation with bunch of QoL functions
*/

#ifndef SPAN_H
#define SPAN_H

#include "core/scanner.h"
#include "core/stringdef.h"
#include <stddef.h>
#include <stdbool.h>

#define NULL_SPAN (Span){0}
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RESET "\x1b[0m"

typedef struct {
  size_t row, col, len;
  bstr str;
  SourceFile* file;
} Span;


/// Makes a zero-length span pointing to the current file position

Span span_begin(SourceFile *file);
/**
  Calculates the difference between the source file and the span given
  and automatically updates length
  @note This should only be called on fresh spans created using span_begin()
  (do not modify span without the utilities)
*/
void span_end(Span *span);

/**
  Increase the length of the span by one
  @note Asserts if the span's end reaches beyond the sourcefile end
*/
void extend_span(Span* span);

/// Highlight the span for diagnostics
void highlight_span(Span span);

/**
  Compares the underlying slice of memory of both spans (along with the length and span.file->name)
  @return true if span1 == span2, false otherwise
*/
bool span_cmp(Span span1, Span span2);

/// Create a span from a string (This automatically populates span.file with the string)
Span str_to_span(bstr str);

/**
 Compare a null-terminated string with a span
 @return true if span == str, false otherwise
*/ 
bool span_str_cmp(Span span, bstr str);

/**
  Copy span's underlying memory slice to the provided buffer
  as null terminated string
*/
bstr dup_span_buf(Span span, bstr buf);

/**
  Advances span.str by one and decreases the length by one
  Example:
  Span:          [123456abcxyz]
  Advanced Span: [23456abcxyz]

  @note Asserts (span.len > 0)
*/
void advance_span(Span* span);

/**
  Shrink the span from end by 1 (effectively decreases span.len by 1)
  @note Asserts (span.len > 0)
*/
void shrink_span(Span* span);

#endif
