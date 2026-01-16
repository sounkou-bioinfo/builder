#ifndef IMPORT_DEFINE_H
#define IMPORT_DEFINE_H

#include "define.h"
#include "parser.h"

void import_defines(Define **defines, Value *paths);
int import_defines_from_line(Define **defines, char *line);

#endif
