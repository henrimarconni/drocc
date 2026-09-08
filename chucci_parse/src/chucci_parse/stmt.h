#ifndef STMT_H_
#define STMT_H_

#include "chucci_parse/parser.h"

void print_stmt(Parser* p, Stmt* stmt);
Stmt* parse_stmt(Parser* p);

#endif
