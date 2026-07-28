/**
  @file
  Utilities related to lexing and scanning in general
*/

#ifndef SCANNER_H
#define SCANNER_H

#include "core/stringdef.h"
#include "core/vmem_arena.h"
#include <stddef.h>
#define EOF (-1)

typedef struct {
  size_t row, col, id, len;
} Position;

/// Contains file name, contents, and the current position of the scanner
typedef struct {
  bstr name;
  bstr contents;
  Position pos;
} SourceFile;

typedef enum {
  SE_OK,
  SE_ERR_CANT_OPEN_FILE,
  SE_ERR_IO,
} ScannerRes;

/**
  Returns the current character and then advances
  Example:
  
  current source file: [abc123xyzhehe]
                              ^ scanner positioni (at 'x')
  nextch(file)         => returns 'x' and advances 
  current source file: [abc123xyzhehe]
                               ^ scanner position (at 'y')
*/
int nextch(SourceFile* file);
/// Returns character at current scanner position
int peekch(SourceFile* file);
/// Returns character just next to the scanner position
int peeknextch(SourceFile* file);
/**
 Matches a string and advances cursor position.
 If matching failed, returns false and rewinds back to original position
*/
bool match_str(SourceFile* file, bstr str);
void skip_space(SourceFile* file);

/**
  Reads file at confpath, populates the source file's contents using the arena
  @return SE_OK on success, SE_ERR_CANT_OPEN_FILE on failure to open file, SE_ERR_IO on other errors
*/
ScannerRes read_file(SourceFile* sf, VMEMArena* arena, bstr confpath);

#endif
