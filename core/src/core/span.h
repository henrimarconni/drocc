/**
  @file
  Span implementation with bunch of QoL functions
*/

#ifndef SPAN_H
#define SPAN_H

#include "core/srcman.h"
#include "core/stringdef.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


StringView span_sv(SourceManager* man, Span span);

/// Highlight the span for diagnostics
void highlight_span(SourceManager* sman, Span span);

/**
  Compare the underlying slice of memory of both spans
  @return true if span1 == span2, false otherwise
*/
bool span_cmp(Span span1, Span span2);

/**
 Compare a null-terminated string with a span
 @return true if span == str, false otherwise
*/
bool span_str_cmp(SourceManager* sman, const Span span, bstr str);

/**
  Copy span's underlying memory slice to the provided buffer
  as null terminated string
  @note If the length of span is greater than the buffer size, this will truncate the output
*/
bstr dup_span_buf(SourceManager* man, Span span, bstr buf, size_t size);

#endif
