#ifndef INCLUDE_H
#define INCLUDE_H

#include "define.h"
#include "plugins.h"

typedef struct {
  char *type;
  char *path;
  char *object;
} Include;

struct Registry_t {
  char *type;
  char *call;
  struct Registry_t *next;
};

typedef struct Registry_t Registry;

char *include_replace(char *line, Plugins *plugins, char *file, Registry **registry);
Registry *initialize_registry();
void push_registry(Registry **registry, char *type, char *call);
void free_registry(Registry *registry);

#endif
