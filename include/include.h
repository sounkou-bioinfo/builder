#ifndef INCLUDE_H
#define INCLUDE_H

#include "define.h"
#include "plugins.h"

typedef struct {
  char *function;
  char *path;
  char *object;
} Include;

char *include_replace(Define **defs, char *line, Plugins *plugins);

#endif
