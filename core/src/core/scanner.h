/**
  @file
  Utilities related to lexing and scanning in general
*/

#ifndef SCANNER_H
#define SCANNER_H

#include "core/srcman.h"
#include "core/stringdef.h"
#include <stddef.h>
#define EOF (-1)


/**
  Returns the current character and then advances
  Example:
  
  current scanner: [abc123xyzhehe]
                              ^ scanner positioni (at 'x')
  nextch(scanner)         => returns 'x' and advances 
  current scanner: [abc123xyzhehe]
                               ^ scanner position (at 'y')
*/
int nextch(SrcScanner* scanner);
/// Returns character at current scanner position
int peekch(SrcScanner* scanner);
/// Returns character just next to the scanner position
int peeknextch(SrcScanner* scanner);
/**
 Matches a string and advances cursor position.
 If matching failed, returns false and rewinds back to original position
*/
bool match_str(SrcScanner* scanner, bstr str);
void skip_space(SrcScanner* scanner);

Span span_begin(SrcScanner* scanner);
void span_end(Span* span, SrcScanner* scanner);

SrcScanner scanner_new(SourceManager* sman, SrcID id);

#endif
