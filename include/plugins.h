#ifndef PLUGINS_H
#define PLUGINS_H

#include "parser.h"
#include "r.h"

struct Plugins_t {
  char *name;
  int setup;
  SEXP obj;
  struct Plugins_t *next;
};

typedef struct Plugins_t Plugins;

Plugins *plugins_init(Value *plugins, char *input, char *output);
int plugins_failed(Plugins *head);

#endif
