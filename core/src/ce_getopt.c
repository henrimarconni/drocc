#include "ce_getopt.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OPTS 256
#define FORMAT_SPACES 20

Opt opts[MAX_OPTS] = {};
int argc = 0;
int curr = 1;
char** argv = NULL;
bstr desc = NULL;
bstr name = NULL;
bstr usage = NULL;

void ce_add_meta(bstr _name, bstr _desc, bstr _usage) {
  desc = _desc;
  name = _name;
  usage = _usage;
}

void ce_initopt(int _argc, char** _argv) {
  argv = _argv;
  argc = _argc;
}

bool is_opt_empty(const Opt* opt) { return memcmp(opt, &opts['\0'], sizeof(Opt)) == 0; }

void ce_addopt(bstr longhand, char shorthand, char val_format, bstr desc) {
  Opt* opt = &opts[shorthand];
  if (!isalnum(shorthand)) {
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
  if (name && desc)
    printf("%s: %s\n", name, desc);
  if (usage)
    printf("Usage: %s\n", usage);

  int format_spaces = FORMAT_SPACES;
  for (size_t i = 0; i < MAX_OPTS; i++) {
    if (!is_opt_empty(&opts[i])) {
      format_spaces -= printf("-%c, --%s", opts[i].shorthand, opts[i].longhand);
      while (format_spaces > 0 && format_spaces--)
        putchar(' ');
      printf(": %s\n", opts[i].desc);
      format_spaces = FORMAT_SPACES;
    }
  }
}

float parse_float(const Opt opt) {
  bstr str = argv[curr++];
  float f = 0;
  bool floated = false;
  float divisor = 1;
  char ch;
  while ((ch = *str++) != '\0') {
    if (ch == '.') {
      if (floated) {
        printf("Bad float: %s\n", argv[curr - 1]);
        exit(-1);
      }
      floated = true;
      continue;
    }
    if (!isdigit(ch)) {
      printf("Error: Expected float in --%s, found: %s\n", opt.longhand, argv[curr - 1]);
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

int parse_int(const Opt opt) {
  bstr str = argv[curr++];
  char ch;
  int num = 0;
  while ((ch = *str++) != '\0') {
    if (!isdigit(ch)) {
      printf("Error: Expected integer in --%s, found: %s\n", opt.longhand, argv[curr - 1]);
      exit(-1);
    }
    num *= 10;
    num += ch - '0';
  }
  return num;
}

void parse_opt(const Opt opt, ParsedOpt* popt) {
  if (opt.val_format == 0) {
    popt->flag = true;
    return;
  }

  if (curr == argc) {
    printf("Error: Expected value of type %c, found nothing in option --%s\n", opt.val_format,
           opt.longhand);
    exit(-1);
  }

  switch (opt.val_format) {
  case 's': {
    popt->s = argv[curr++];
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
    printf("Invalid value specifier: %c\n", opt.val_format);
    abort();
  }
  }
}

bool ce_getopt(char* ch, ParsedOpt* popt) {
  *popt = (ParsedOpt){};
  if (!argv || !argc) {
    printf("Error: use ce_initopt before ce_getopt\n");
    abort();
  }
  if (curr == argc)
    return false;

  bstr str = argv[curr++];
  if (str[0] != '-') {
    *ch = CE_PLAIN_VALUE;
    popt->s = str;
    return true;
  }

  size_t len = strlen(str);
  if (len == 2) {
    char shorthand = str[1];
    const Opt opt = opts[shorthand];
    *ch = shorthand;
    parse_opt(opt, popt);
    return true;
  }

  if (str[1] != '-') {
    printf("Error: Unxpected option `%s`\n", str);
    exit(-1);
  }

  for (size_t i = 0; i < MAX_OPTS; i++) {
    if (opts[i].longhand && strcmp(opts[i].longhand, str + 2) == 0) {
      *ch = opts[i].shorthand;
      parse_opt(opts[i], popt);
      return true;
    }
  }

  printf("Error: Unknown argument %s\n", str);
  exit(-1);
}
