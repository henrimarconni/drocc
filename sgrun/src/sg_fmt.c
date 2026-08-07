#include "sgrun/sg_fmt.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void print_heading(char ch, bstr fmt, ...) {
  char title[512];

  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(title, sizeof(title), fmt, args);
  va_end(args);

  size_t used = len + 4; // two spaces on each side
  if (used >= HEADING_WIDTH) {
    size_t max_title = HEADING_WIDTH - 4;

    if (max_title > 3) {
      memcpy(title + max_title - 3, "...", 3);
      title[max_title] = '\0';
    }

    printf("  %s  \n", title);
    return;
  }
  size_t dash_total = (used >= HEADING_WIDTH) ? 0 : HEADING_WIDTH - used;

  size_t left = dash_total / 2;
  size_t right = dash_total - left;

  for (size_t i = 0; i < left; i++)
    putchar(ch);

  printf("  %s  ", title);

  for (size_t i = 0; i < right; i++)
    putchar(ch);

  putchar('\n');
}
