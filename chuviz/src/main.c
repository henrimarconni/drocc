#define TB_OPT_ATTR_W 64
#define TB_IMPL
#include "thirdparty/termbox2.h"
#undef TB_IMPL
#include "chuviz/lexer_tab.h"
#include "core/ce_getopt.h"
#include "core/cli_diag.h"
#include "core/srcman.h"
#include "core/string_interner.h"
#include "core/vmem_arena.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

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

void event_loop(LexerTab* tab) {
  tb_init();
  tb_set_output_mode(TB_OUTPUT_TRUECOLOR);

  signal(SIGINT, cleanup_and_exit);
  signal(SIGTERM, cleanup_and_exit);
  signal(SIGSEGV, cleanup_and_exit);

  while (true) {
    tb_clear();
    render_lexert(tab);
    tb_present();

    struct tb_event ev;
    tb_poll_event(&ev);
    lexertab_input(tab, &ev);
    if (ev.key == TB_KEY_ESC || ev.ch == 'q')
      break;
  }

  tb_shutdown();
}

int main(int argc, char** argv) {
  bstr file = NULL;
  parse_args(argc, argv, &file);

  SourceManager sman = sman_new();
  VMEMArena* arena = vmarena_new(128 * 1024);
  SrcID srcid = sman_open(&sman, file, arena);

  if (srcid == INVALID_SRC_ID) {
    vmarena_free(arena);
    sman_free(&sman);
    clid_throw_diag(CLID_ERROR, -1, "Cannot open file: %s", file);
  }

  StringInterner* interner = interner_new(arena);
  LexerTab tab = lexertab_init(&sman, interner, srcid);
  event_loop(&tab);

  return 0;
}
