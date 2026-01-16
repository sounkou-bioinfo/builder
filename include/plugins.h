#ifndef PLUGINS_H
#define PLUGINS_H

#include "parser.h"

struct Plugins_t {
  char *name;
  int setup;
  struct Plugins_t *next;
};

typedef struct Plugins_t Plugins;

Plugins *plugins_init(Value *plugins);
int plugins_failed(Plugins *head);

#endif
