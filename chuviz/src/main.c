#include "chuviz/lexer_tab.h"
#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/signals.h"
#include "libterm/libterm.h"
#include <stdio.h>
#include <stdlib.h>

static void handler(int sig) {
  lt_shutdown();
  printf("chuviz exiting with signal: %d\n", sig);
  exit(1);
}

static void parse_args(int argc, char** argv, bstr* file) {
  register_crash_handlers(handler);
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

static void event_loop(LexerTab* tab) {
  lt_init();
  lt_set_output_mode(LT_OUTPUT_TRUECOLOR);

  while (true) {
    lt_clear();
    render_lexert(tab);
    lt_present();

    struct lt_event ev;
    lt_poll_event(&ev);
    lexertab_input(tab, &ev);
    if (ev.key == LT_KEY_ESC || ev.ch == 'q')
      break;
  }

  lt_shutdown();
}

int main(int argc, char** argv) {
  bstr file = NULL;
  parse_args(argc, argv, &file);

  LexerTab* tab = lexertab_init(file);
  if (!tab)
    clid_throw_diag(CLID_ERROR, -1, "Cannot open file: %s", file);

  event_loop(tab);

  lexertab_free(tab);
  return 0;
}
