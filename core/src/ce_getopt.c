#include "core/ce_getopt.h"
#include "core/strutils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OPTS 62
#define FORMAT_SPACES 20

typedef struct {
  Opt opts[MAX_OPTS];
  int argc;
  int curr;
  char** argv;
  bstr desc;
  bstr name;
  bstr usage;
} GetoptData;

static GetoptData ce_data = {
    .curr = 1,
    .argc = 0,
    .argv = NULL,
    .desc = NULL,
    .name = NULL,
    .usage = NULL,
    .opts = {{0}},
};

/*
  Maps [a..z], [A..Z], and [0..9] to array index [0..61]
*/
static int ce_translate(char ch) {
  if (ch >= 'a' && ch <= 'z')
    return ch - 'a';
  if (ch >= 'A' && ch <= 'Z')
    return 26 + (ch - 'A');
  if (ch >= '0' && ch <= '9')
    return 52 + (ch - '0');
  return -1;
}

static bool is_opt_empty(const Opt* opt) { return opt->shorthand == 0 && opt->longhand == NULL; }

static Opt* get_opt(char ch) {
  int idx = ce_translate(ch);
  if (idx < 0)
    return NULL;
  return &ce_data.opts[idx];
}

void ce_add_meta(bstr name, bstr desc, bstr usage) {
  ce_data.desc = desc;
  ce_data.name = name;
  ce_data.usage = usage;
}

void ce_initopt(int argc, char** argv) {
  ce_data.argv = argv;
  ce_data.argc = argc;
  ce_data.curr = 1;
}

void ce_addopt(bstr longhand, char shorthand, char val_format, bstr desc) {
  Opt* opt = get_opt(shorthand);
  if (!opt) {
    printf("Error: shorthand isn't an alphanumeric character\n");
    abort();
  }
  if (!is_opt_empty(opt)) {
    printf("Error: shorthand '%c' already exists\n", shorthand);
    abort();
  }
  *opt = (Opt){longhand, shorthand, val_format, desc};
}

void ce_printhelp(void) {
  if (ce_data.name && ce_data.desc)
    printf("%s: %s\n", ce_data.name, ce_data.desc);
  if (ce_data.usage)
    printf("Usage: %s\n", ce_data.usage);

  for (size_t i = 0; i < MAX_OPTS; i++) {
    Opt* opt = &ce_data.opts[i];
    if (!is_opt_empty(opt)) {
      int printed = printf("-%c, --%s", opt->shorthand, opt->longhand);
      int spaces = FORMAT_SPACES - printed;
      while (spaces-- > 0) {
        putchar(' ');
      }
      printf(": %s\n", opt->desc);
    }
  }
}

static void parse_opt(const Opt* opt, ParsedOpt* popt) {
  if (opt->val_format == 0) {
    popt->flag = true;
    return;
  }

  if (ce_data.curr == ce_data.argc) {
    printf(
        "Error: Expected value of type %c, found nothing in option --%s\n",
        opt->val_format,
        opt->longhand);
    exit(-1);
  }

  switch (opt->val_format) {
  case 's': {
    popt->s = ce_data.argv[ce_data.curr++];
    break;
  }
  case 'd': {
    parse_int(&popt->d, ce_data.argv[ce_data.curr++]);
    break;
  }
  case 'f': {
    parse_float(&popt->f, ce_data.argv[ce_data.curr++]);
    break;
  }
  default: {
    printf("Invalid value specifier: %c\n", opt->val_format);
    abort();
  }
  }
}

bool ce_getopt(char* ch, ParsedOpt* popt) {
  *popt = (ParsedOpt){};
  if (!ce_data.argv || !ce_data.argc) {
    printf("Error: use ce_initopt before ce_getopt\n");
    abort();
  }
  if (ce_data.curr == ce_data.argc)
    return false;

  bstr str = ce_data.argv[ce_data.curr++];

  if (str[0] != '-' || strcmp(str, "-") == 0) {
    *ch = CE_PLAIN_VALUE;
    popt->s = str;
    return true;
  }

  size_t len = strlen(str);
  if (len == 2) {
    char shorthand = str[1];
    Opt* opt = get_opt(shorthand);
    if (!opt) {
      printf("Error: invalid argument: -%c\n", shorthand);
      exit(-1);
    }
    *ch = shorthand;
    parse_opt(opt, popt);
    return true;
  }

  if (str[1] != '-') {
    printf("Error: Unexpected option `%s`\n", str);
    exit(-1);
  }

  for (size_t i = 0; i < MAX_OPTS; i++) {
    Opt* opt = &ce_data.opts[i];
    if (opt->longhand && strcmp(opt->longhand, str + 2) == 0) {
      *ch = opt->shorthand;
      parse_opt(opt, popt);
      return true;
    }
  }

  printf("Error: Unknown argument %s\n", str);
  exit(-1);
}
