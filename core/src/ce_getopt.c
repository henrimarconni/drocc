#include "ce_getopt.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <ctype.h>
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
    .opts = {0},
};

static Opt nullopt = {0};

/*
  Maps [A..Z] [a..z] and '0-9' to a number [0..61]
  Returns -1 on invalid character
*/
static int ce_translate(char ch) {
  if (ch <= 'z' && ch >= 'a')
    return ch - 'a';
  else if (ch <= 'Z' && ch >= 'A')
    return 26 + ch - 'A';
  else if (ch <= '9' && ch >= '0')
    return 26 + 26 + ch - '9';
  else
    return -1;
}

static bool is_opt_empty(const Opt* opt) { return memcmp(opt, &nullopt, sizeof(Opt)) == 0; }

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
}

void ce_addopt(bstr longhand, char shorthand, char val_format, bstr desc) {
  Opt* opt = get_opt(shorthand);
  if (!opt) {
    printf("Error: shorthand isnt an alphabet or number\n");
    abort();
  }
  if (!is_opt_empty(opt)) {
    printf("Error: shorthand already exists\n");
    abort();
  }
  *opt = (Opt){longhand, shorthand, val_format, desc};
}

void ce_printhelp() {
  if (ce_data.name && ce_data.desc)
    printf("%s: %s\n", ce_data.name, ce_data.desc);
  if (ce_data.usage)
    printf("Usage: %s\n", ce_data.usage);

  int format_spaces = FORMAT_SPACES;
  for (size_t i = 0; i < MAX_OPTS; i++) {
    Opt* opt = &ce_data.opts[i];
    if (!is_opt_empty(opt)) {
      format_spaces -= printf("-%c, --%s", opt->shorthand, opt->longhand);
      while (format_spaces > 0 && format_spaces--)
        putchar(' ');
      printf(": %s\n", opt->desc);
      format_spaces = FORMAT_SPACES;
    }
  }
}

static float parse_float(const Opt* opt) {
  bstr str = ce_data.argv[ce_data.curr++];
  float f = 0;
  bool floated = false;
  float divisor = 1;
  char ch;
  while ((ch = *str++) != '\0') {
    if (ch == '.') {
      if (floated) {
        printf("Bad float: %s\n", ce_data.argv[ce_data.curr - 1]);
        exit(-1);
      }
      floated = true;
      continue;
    }
    if (!isdigit(ch)) {
      printf("Error: Expected float in --%s, found: %s\n", opt->longhand,
             ce_data.argv[ce_data.curr - 1]);
      exit(-1);
    }
    if (!floated) {
      f = (f * 10.0) + (ch - '0');
    } else {
      divisor *= 10.0;
      f = f + ((ch - '0') / divisor);
    }
  }
  return f;
}

static int parse_int(const Opt* opt) {
  bstr str = ce_data.argv[ce_data.curr++];
  char ch;
  int num = 0;
  while ((ch = *str++) != '\0') {
    if (!isdigit(ch)) {
      printf("Error: Expected integer in --%s, found: %s\n", opt->longhand,
             ce_data.argv[ce_data.curr - 1]);
      exit(-1);
    }
    num *= 10;
    num += ch - '0';
  }
  return num;
}

static void parse_opt(const Opt* opt, ParsedOpt* popt) {
  if (opt->val_format == 0) {
    popt->flag = true;
    return;
  }

  if (ce_data.curr == ce_data.argc) {
    printf("Error: Expected value of type %c, found nothing in option --%s\n", opt->val_format,
           opt->longhand);
    exit(-1);
  }

  switch (opt->val_format) {
  case 's': {
    popt->s = ce_data.argv[ce_data.curr++];
    break;
  }
  case 'd': {
    popt->d = parse_int(opt);
    break;
  }
  case 'f': {
    popt->f = parse_float(opt);
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
  if (str[0] != '-') {
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
      return false;
    }
    *ch = shorthand;
    parse_opt(opt, popt);
    return true;
  }

  if (str[1] != '-') {
    printf("Error: Unxpected option `%s`\n", str);
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
