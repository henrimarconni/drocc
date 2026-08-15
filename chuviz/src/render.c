#include "chuviz/render.h"
#include "libterm/libterm.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void render_rect(Rect rect, uint32_t border_fg) {
  if (rect.w < 2 || rect.h < 2)
    return;

  int ex = rect.start.x + rect.w - 1;
  int ey = rect.start.y + rect.h - 1;

  // Corners
  lt_set_cell(rect.start.x, rect.start.y, 0x250C, border_fg, LT_DEFAULT); // ┌
  lt_set_cell(ex, rect.start.y, 0x2510, border_fg, LT_DEFAULT);           // ┐
  lt_set_cell(rect.start.x, ey, 0x2514, border_fg, LT_DEFAULT);           // └
  lt_set_cell(ex, ey, 0x2518, border_fg, LT_DEFAULT);                     // ┘

  // Horizontal edges
  for (int i = 1; i < rect.w - 1; ++i) {
    lt_set_cell(rect.start.x + i, rect.start.y, 0x2500, border_fg, LT_DEFAULT); // ─
    lt_set_cell(rect.start.x + i, ey, 0x2500, border_fg, LT_DEFAULT);           // ─
  }

  // Vertical edges
  for (int i = 1; i < rect.h - 1; ++i) {
    lt_set_cell(rect.start.x, rect.start.y + i, 0x2502, border_fg, LT_DEFAULT); // │
    lt_set_cell(ex, rect.start.y + i, 0x2502, border_fg, LT_DEFAULT);           // │
  }
}

void render_boxedtext(BoxedText boxtext, uint32_t border_fg) {
  render_rect(boxtext.rect, border_fg);

  int len = (int)strlen(boxtext.str);
  int center_x = boxtext.rect.start.x + (boxtext.rect.w - len) / 2;
  int center_y = boxtext.rect.start.y + (boxtext.rect.h / 2);

  lt_printf(center_x, center_y, border_fg | LT_BOLD, LT_DEFAULT, "%s", boxtext.str);
}

void render_boxovtext(BoxOverlayText boxov, uint32_t border_fg) {
  render_rect(boxov.rect, border_fg);

  if (!boxov.str)
    return;

  // Print over the top-left edge, offset by 2 cells (┌─text)
  lt_printf(boxov.rect.start.x + 2, boxov.rect.start.y, border_fg, LT_DEFAULT, "%s", boxov.str);
}

Rect get_window_rect() {
  Rect r = {.start = {0, 0}, .w = lt_width(), .h = lt_height()};
  return r;
}

void vdivide_rect(Rect rect, Rect* panes, size_t count, int* percentage) {
  Vec2 start = rect.start;
  for (size_t i = 0; i < count; i++) {
    Rect* curr = &panes[i];
    curr->start = start;
    curr->h = rect.h;
    curr->w = rect.w * percentage[i] / 100;
    start.x += rect.w * percentage[i] / 100;

    assert(start.x <= rect.start.x + rect.w);
  }
}

RectPrintCtx rect_print_init(Rect* rect) {
  RectPrintCtx ctx = {0};
  ctx.rect = rect;
  ctx.pos = rect->start;
  return ctx;
}

void rect_printf(RectPrintCtx* ctx, uint32_t fg, uint32_t bg, const char* fmt, ...) {
  char buf[4096];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  size_t i = 0;
  while (buf[i]) {
    if (buf[i] == '\n') {
      ctx->pos.y++;
      ctx->pos.x = ctx->rect->start.x;
      i++;
      continue;
    }

    if (ctx->pos.x >= ctx->rect->start.x + ctx->rect->w) {
      ctx->pos.y++;
      ctx->pos.x = ctx->rect->start.x;
    }

    if (ctx->pos.y >= ctx->rect->start.y + ctx->rect->h)
      break;

    lt_set_cell(ctx->pos.x, ctx->pos.y, (lt_uchar)buf[i], fg, bg);

    ctx->pos.x++;
    i++;
  }
}

Rect bordered_rect(Rect rect) {
  assert(rect.w > 2 && rect.h > 2);
  rect.h -= 2;
  rect.w -= 2;
  rect.start.x += 1;
  rect.start.y += 1;
  return rect;
}
