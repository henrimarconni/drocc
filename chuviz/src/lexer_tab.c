#include "chucci_lex/lexer.h"
#include "chucci_lex/token.h"
#include "chucci_lex/token_stream.h"
#include "chuviz/lexer_tab.h"
#include "chuviz/render.h"
#include "chuviz/theme.h"
#include "core/srcman.h"
#include "core/vec.h"
#include "libterm/libterm.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TAB_WIDTH 2

static inline void print_line_start(RectPrintCtx* srcctx, int max_line_size, size_t line_start) {
  rect_printf(srcctx, LT_DEFAULT, LT_DEFAULT, "%*zu  ", max_line_size, line_start + 1);
}

uint64_t token_color(TokenKind kind) {
  if (kind < _keyword_count)
    return THEME_CATPPUCCIN.keyword_fg | LT_BOLD;
  if (kind < _op_sep_end)
    return THEME_CATPPUCCIN.op_sep_fg;
  if (kind == TOK_STR || kind == TOK_ANGLE)
    return THEME_CATPPUCCIN.string_fg;
  if (kind == TOK_IDENT)
    return THEME_CATPPUCCIN.ident_fg;
  if (kind == TOK_VAL)
    return THEME_CATPPUCCIN.value_fg;
  assert(false);
}

typedef struct {
  SMSource* file;
  RectPrintCtx* srcctx;
  LexerTab* tab;
  int scope_level;
  bool is_pp_cmd;
} PrintSrcCtx;

/// Print the source
static inline void print_src(PrintSrcCtx* ctx) {
  char tmp[16];
  int max_line_size = snprintf(tmp, sizeof(tmp), "%zu", ctx->file->offsets.n);

  size_t line_start = ctx->tab->scroll_y;
  print_line_start(ctx->srcctx, max_line_size, line_start);

  // loop through all tokens
  for (size_t i = 0; i < ctx->tab->tokens.n; i++) {
    Token* token = &ctx->tab->tokens.get[i];
    if (token->span.offset < ctx->file->offsets.get[ctx->tab->scroll_y])
      continue;

    // Formatting
    if (token->kind == SEP_LCURLY)
      ctx->scope_level++;
    if (token->kind == SEP_RCURLY && ctx->scope_level > 0)
      ctx->scope_level--;

    // if newline, print line start
    while (line_start + 1 < ctx->file->offsets.n &&
           token->span.offset >= ctx->file->offsets.get[line_start + 1]) {
      line_start++;
      rect_printf(ctx->srcctx, LT_DEFAULT, LT_DEFAULT, "\n");
      print_line_start(ctx->srcctx, max_line_size, line_start);

      int spaces = ctx->scope_level * TAB_WIDTH;
      while (spaces--)
        rect_printf(ctx->srcctx, LT_DEFAULT, LT_DEFAULT, " ");
    }

    // if (token->kind == SEP_NEWLINE)
    //   continue;

    if (token->kind == SEP_NEWLINE && i == ctx->tab->selected) {
      ctx->is_pp_cmd = false;
      rect_printf(
          ctx->srcctx, token_color(token->kind) | LT_REVERSE, LT_DEFAULT | LT_REVERSE, "#newline");
      continue;
    } else if (token->kind == SEP_NEWLINE) {
      rect_printf(ctx->srcctx, token_color(token->kind), LT_DEFAULT, "#newline");
      continue;
    }

    // If the current token is selected, write the line
    // to cursor_line
    if (i == ctx->tab->selected) {
      ctx->is_pp_cmd = false;
      rect_printf(
          ctx->srcctx,
          token_color(token->kind) | LT_REVERSE,
          LT_DEFAULT | LT_REVERSE,
          "%.*s",
          token->span.len,
          ctx->file->contents + token->span.offset);
    }
    // if we see #, mark the next token as pp_cmd
    // so that it gets the same highlighting as #
    // instead of ident highlighting
    else if (!ctx->is_pp_cmd) {
      rect_printf(
          ctx->srcctx,
          token_color(token->kind),
          LT_DEFAULT,
          "%.*s",
          token->span.len,
          ctx->file->contents + token->span.offset);
    }
    // found the marked pp_cmd token, highlight
    // it with same color as #
    else {
      ctx->is_pp_cmd = false;
      rect_printf(
          ctx->srcctx,
          token_color(OP_PREPROCESS),
          LT_DEFAULT,
          "%.*s",
          token->span.len,
          ctx->file->contents + token->span.offset);
    }

    if (token->kind == OP_PREPROCESS)
      ctx->is_pp_cmd = true;
    else
      rect_printf(ctx->srcctx, LT_DEFAULT, LT_DEFAULT, " ");
  }
}

void print_token_info(RectPrintCtx* ctx, Token* token, SMSpanInfo info) {
  char buf[16] = {0};
  memset(buf, ' ', TAB_WIDTH);
  rect_printf(
      ctx,
      LT_DEFAULT,
      LT_DEFAULT,
      "token.span = \n%s.offset = %d\n%s.len = %d\n%s.srcid = %d\n\n",
      buf,
      token->span.offset,
      buf,
      token->span.len,
      buf,
      token->span.srcid);

  if (token->kind == SEP_NEWLINE)
    rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "token.kind = `NEWLINE`\n");
  else
    rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "token.kind = `%s`\n", tok_to_str[token->kind]);

  if (token->kind == TOK_IDENT)
    rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "token.ident = %d\n\n", token->ident);
  else
    rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "\n\n");

  rect_printf(
      ctx,
      LT_DEFAULT,
      LT_DEFAULT,
      "SMSpanInfo = \n%s.row = %d\n%s.col = %d\n",
      buf,
      info.row,
      buf,
      info.col);

  if (token->kind == SEP_NEWLINE)
    rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "%s.sv = `\\n`\n", buf);
  else
    rect_printf(
        ctx, LT_DEFAULT, LT_DEFAULT, "%s.sv = `%.*s`\n", buf, (int)info.sv.len, info.sv.str);

  // print highlighted line
  rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "In line %d: \n", info.row);
  rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "%.*s", info.col, info.sv.str - info.col);
  rect_printf(
      ctx, token_color(token->kind) | LT_BOLD, LT_DEFAULT, "%.*s", info.sv.len, info.sv.str);

  size_t id = info.sv.len;
  while (info.sv.str[id] && info.sv.str[id] != '\n')
    id++;
  rect_printf(ctx, LT_DEFAULT, LT_DEFAULT, "%.*s\n", id - info.sv.len, info.sv.str + info.sv.len);
}

void render_lexert(LexerTab* tab) {
  Rect window = get_window_rect();
  Rect panes[2];

  SMSource* file = &tab->sman->sources.get[tab->srcid];

  vdivide_rect(window, panes, 2, (int[]){tab->percentage1, 100 - tab->percentage1});

  // setup divided panes
  BoxOverlayText srcpane = {0};
  srcpane.str = file->name;
  srcpane.rect = panes[0];

  BoxOverlayText tokeninfo = {0};
  tokeninfo.str = "Token Info";
  tokeninfo.rect = panes[1];

  // make sure the border doesnt overlap
  panes[0] = bordered_rect(panes[0]);
  panes[1] = bordered_rect(panes[1]);

  Token* active_token = &tab->tokens.get[tab->selected];
  SMSpanInfo info = sman_info(tab->sman, active_token->span);

  size_t pane_height = panes[0].h;
  size_t top_margin = pane_height / 5;          // 20% threshold
  size_t bottom_margin = (pane_height * 4) / 5; // 80% threshold

  // Push the camera UP if the token is too high
  if (info.row < tab->scroll_y + top_margin) {
    if (info.row > top_margin)
      tab->scroll_y = info.row - top_margin;
    else
      tab->scroll_y = 0;
  }
  // Push the camera DOWN if the token is too low
  else if (info.row > tab->scroll_y + bottom_margin) {
    tab->scroll_y = info.row - bottom_margin;

    if (file->offsets.n > pane_height) {
      if (tab->scroll_y > file->offsets.n - pane_height)
        tab->scroll_y = file->offsets.n - pane_height;
    } else
      tab->scroll_y = 0;
  }

  PrintSrcCtx ctx = {0};
  RectPrintCtx srcctx = rect_print_init(&panes[0]);
  ctx.srcctx = &srcctx;
  ctx.file = file;
  ctx.tab = tab;

  print_src(&ctx);

  RectPrintCtx tokctx = rect_print_init(&panes[1]);
  print_token_info(&tokctx, active_token, info);

  render_boxovtext(tokeninfo, LT_DEFAULT);
  render_boxovtext(srcpane, LT_DEFAULT);
}

void lexertab_input(LexerTab* tab, struct lt_event* event) {
  // pane resizing
  if (event->mod == LT_MOD_SHIFT) {
    if (event->key == LT_KEY_ARROW_RIGHT && tab->percentage1 < 80)
      tab->percentage1 += 5;
    else if (event->key == LT_KEY_ARROW_LEFT && tab->percentage1 > 20)
      tab->percentage1 -= 5;

    return;
  }

  // token navigation
  if (event->key == LT_KEY_ARROW_LEFT || event->key == LT_KEY_ARROW_UP || event->ch == 'h') {
    if (tab->selected > 0)
      tab->selected -= 1;
  } else if (
      event->key == LT_KEY_ARROW_RIGHT || event->key == LT_KEY_ARROW_DOWN || event->ch == 'l') {
    if (tab->selected + 1 < tab->tokens.n)
      tab->selected += 1;
  }
}

LexerTab lexertab_init(SourceManager* sman, StringInterner* interner, SrcID srcid) {
  LexerTab tab = {0};
  tab.interner = interner;
  tab.srcid = srcid;
  tab.sman = sman;
  tab.percentage1 = 60;

  TokenStream ts = lexer_new(sman, srcid, interner);
  Token token = ts_next(&ts);

  while (token.kind != TOK_EOF) {
    // if (token.kind != SEP_NEWLINE)
    vec_push(tab.tokens, token);
    token = ts_next(&ts);
  }

  ts_free(&ts);
  return tab;
}
