#include "render.h"
#include <signal.h>
#include <unistd.h>
#define TB_IMPL
#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "thirdparty/termbox2.h"
#include <stdlib.h>

void cleanup_and_exit(int sig) {
  tb_shutdown();
  exit(sig);
}

void parse_args(int argc, char** argv, bstr* file) {
  ce_initopt(argc, argv);
  ce_add_meta("chuviz", "The chucci visualizer", "./chuviz file.c");
  ce_addopt("help", 'h', 0, "Print help");

  char ch;
  ParsedOpt popt;

  while (ce_getopt(&ch, &popt)) {
    switch (ch) {
    case CE_PLAIN_VALUE:
      if (*file)
        clid_throw_diag(CLID_ERROR, -1, "One file at a time, kudasai");
      *file = popt.s;
      break;
    case 'h':
      ce_printhelp();
      exit(0);
    default:
      clid_throw_diag(CLID_ERROR, -1, "Invalid argument");
    }
  }

  if (!file)
    clid_throw_diag(CLID_ERROR, -1, "Please specify the file :P");
}

void event_loop() {
  tb_init();

  signal(SIGINT, cleanup_and_exit);
  signal(SIGTERM, cleanup_and_exit);
  signal(SIGSEGV, cleanup_and_exit);

  Cursor c = {0};
  while (true) {
    Rect window = get_window_rect();
    BoxOverlayText titlebar = {0};
    titlebar.rect = window;
    titlebar.rect.h = 3;
    titlebar.str = " chuviz: chucci visualizer ";
    window.start.y += 3;
    window.h -= 3;
    c.bounds = window;
    c.bounds.start.x++;
    c.bounds.start.y++;
    c.bounds.w -= 2;
    c.bounds.h -= 2;

    tb_clear();
    render_boxovtext(titlebar, TB_DEFAULT);
    render_rect(window, TB_DEFAULT);
    render_cursor(&c);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);
    if (ev.type == TB_EVENT_KEY) {
      if (ev.key == TB_KEY_ARROW_RIGHT || ev.ch == 'l')
        c.pos.x += 1;
      if (ev.key == TB_KEY_ARROW_LEFT && c.pos.x > 0 || ev.ch == 'h')
        c.pos.x -= 1;
      if (ev.key == TB_KEY_ARROW_UP && c.pos.y > 0 || ev.ch == 'k')
        c.pos.y -= 1;
      if (ev.key == TB_KEY_ARROW_DOWN || ev.ch == 'j')
        c.pos.y += 1;
      if (ev.key == TB_KEY_ESC || ev.ch == 'q')
        break;
    }
  }

  tb_shutdown();
}

int main(int argc, char** argv) {
  bstr file = NULL;
  parse_args(argc, argv, &file);
  event_loop();
  return 0;
}
