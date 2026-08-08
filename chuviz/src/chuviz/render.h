#ifndef RENDER_H_
#define RENDER_H_

#include "core/stringdef.h"
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


void render_boxedtext(BoxedText boxtext, uint16_t border_fg);
void render_boxovtext(BoxOverlayText boxov, uint16_t border_fg);
void render_rect(Rect rect, uint16_t border_fg);

Rect get_window_rect();

void render_cursor(Cursor* c);

#endif
