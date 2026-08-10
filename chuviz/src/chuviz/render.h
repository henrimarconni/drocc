#ifndef RENDER_H_
#define RENDER_H_

#include "core/stringdef.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  int x;
  int y;
} Vec2;

/*
  ~ start here
| ┌─────w──────┐|
| h            │|
| └────────────┘|
*/
typedef struct {
  Vec2 start;
  int w;
  int h;
} Rect;

/*
| ┌────────────┐|
| │   TITLE    │|
| └────────────┘|
*/
typedef struct {
  bstr str;
  Rect rect;
} BoxedText;

/*
| ┌─cool text──┐|
| │            │|
| └────────────┘|
*/
typedef struct {
  bstr str;
  Rect rect;
} BoxOverlayText;

typedef struct {
  Vec2 pos;
  Rect bounds;
  bool hidden;
} Cursor;

typedef struct {
  Vec2 pos;
  Rect* rect;
} RectPrintCtx;

void render_boxedtext(BoxedText boxtext, uint64_t border_fg);
void render_boxovtext(BoxOverlayText boxov, uint64_t border_fg);
void render_rect(Rect rect, uint64_t border_fg);

/// vertically divide the rect into separate panes, with given percentages of area
void vdivide_rect(Rect rect, Rect* panes, size_t count, int* percentage);

RectPrintCtx rect_print_init(Rect* rect);
void rect_printf(RectPrintCtx* ctx, uint64_t fg, uint64_t bg, const char* fmt, ...);

Rect get_window_rect();
Rect bordered_rect(Rect rect);

void render_cursor(Cursor* c);

#endif
