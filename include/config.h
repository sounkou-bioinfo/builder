#ifndef CONFIG_H
#define CONFIG_H

#include "parser.h"
#include "plugins.h"

typedef struct {
  int argc;
  char **argv;
  char *input;
  char *output;
  Value *imports;
  Value *plugins_str;
  char *prepend;
  char *append;
  int deadcode;
  int must_clean;
  int sourcemap;
  int watch;
  Plugins *plugins;
} BuildContext;

int has_config();
BuildContext *get_config();
void free_config(BuildContext *ctx);

#endif
