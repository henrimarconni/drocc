#include "chuviz/render.h"
#include "thirdparty/termbox2.h"
#include <string.h>

void render_rect(Rect rect, uint16_t border_fg) {
  if (rect.w < 2 || rect.h < 2)
    return;

  int ex = rect.start.x + rect.w - 1;
  int ey = rect.start.y + rect.h - 1;

  // Corners
  tb_set_cell(rect.start.x, rect.start.y, 0x250C, border_fg, TB_DEFAULT); // ┌
  tb_set_cell(ex, rect.start.y, 0x2510, border_fg, TB_DEFAULT);           // ┐
  tb_set_cell(rect.start.x, ey, 0x2514, border_fg, TB_DEFAULT);           // └
  tb_set_cell(ex, ey, 0x2518, border_fg, TB_DEFAULT);                     // ┘

  // Horizontal edges
  for (int i = 1; i < rect.w - 1; ++i) {
    tb_set_cell(rect.start.x + i, rect.start.y, 0x2500, border_fg, TB_DEFAULT); // ─
    tb_set_cell(rect.start.x + i, ey, 0x2500, border_fg, TB_DEFAULT);           // ─
  }

  // Vertical edges
  for (int i = 1; i < rect.h - 1; ++i) {
    tb_set_cell(rect.start.x, rect.start.y + i, 0x2502, border_fg, TB_DEFAULT); // │
    tb_set_cell(ex, rect.start.y + i, 0x2502, border_fg, TB_DEFAULT);           // │
  }
}

void render_boxedtext(BoxedText boxtext, uint16_t border_fg) {
  render_rect(boxtext.rect, border_fg);

  if (!boxtext.str) {
    tb_printf(0, 0, border_fg, TB_DEFAULT, "oye hoye sugma");
    return;
  }

  int len = strlen(boxtext.str);
  int center_x = boxtext.rect.start.x + (boxtext.rect.w - len) / 2;
  int center_y = boxtext.rect.start.y + (boxtext.rect.h / 2);

  tb_printf(center_x, center_y, border_fg | TB_BOLD, TB_DEFAULT, "%s", boxtext.str);
}

void render_boxovtext(BoxOverlayText boxov, uint16_t border_fg) {
  render_rect(boxov.rect, border_fg);

  if (!boxov.str)
    return;

  // Print over the top-left edge, offset by 2 cells (┌─text)
  tb_printf(
      boxov.rect.start.x + 2, boxov.rect.start.y, border_fg | TB_BOLD, TB_DEFAULT, "%s", boxov.str);
}

Rect get_window_rect() {
  Rect r = {.start = {0, 0}, .w = tb_width(), .h = tb_height()};
  return r;
}

void render_cursor(Cursor* c) {
  if (c->hidden)
    return;

  if (c->pos.x < c->bounds.start.x)
    c->pos.x = c->bounds.start.x;
  if (c->pos.y < c->bounds.start.y)
    c->pos.y = c->bounds.start.y;

  int max_x = c->bounds.start.x + c->bounds.w - 1;
  int max_y = c->bounds.start.y + c->bounds.h - 1;

  if (c->pos.x > max_x)
    c->pos.x = max_x;
  if (c->pos.y > max_y)
    c->pos.y = max_y;

  int tw = tb_width();
  int th = tb_height();

  if (c->pos.x >= 0 && c->pos.x < tw && c->pos.y >= 0 && c->pos.y < th) {
    struct tb_cell* cell = tb_cell_buffer() + (c->pos.y * tw + c->pos.x);
    cell->fg |= TB_REVERSE;
  }
}
