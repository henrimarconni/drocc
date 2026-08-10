#ifndef THEME_H_
#define THEME_H_

#include <stdint.h>
#include "thirdparty/termbox2.h"

typedef struct {
  uint32_t bg;
  uint32_t ident_fg;
  uint32_t value_fg;
  uint32_t keyword_fg;
  uint32_t op_sep_fg;
  uint32_t string_fg;
} CodeTheme;

static const CodeTheme THEME_CATPPUCCIN = {
    .bg = 0x1A1B26,         // The deep, cool blue-black background
    .ident_fg = TB_DEFAULT,   // The light slate-blue for variables like 'boxtext' and 'len'
    .value_fg = 0xFF9E64,   // The bright orange for numbers like '1', '2', and '0x2502'
    .keyword_fg = 0xBB9AF7, // The soft purple for 'void', 'int', 'if', 'return'
    .op_sep_fg = 0x89DDFF,  // The cyan-tinted white for brackets, parentheses, and operators
    .string_fg = 0x9ECE6A,  // The crisp green for string literals like "%s"
};

#endif
