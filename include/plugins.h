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
char *plugins_call(Plugins *head, char *fn, char *str, char *file);
char *plugins_call_include(Plugins *head, char *type, char *path, char *object, char *file);
void free_plugins(Plugins *head);

#endif
