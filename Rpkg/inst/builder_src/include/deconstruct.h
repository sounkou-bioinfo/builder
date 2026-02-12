#ifndef DECONSTRUCT_H
#define DECONSTRUCT_H

typedef struct Var_t {
  char *value;
  struct Var_t *next;
} Var;

char *deconstruct_replace(char *line);

#endif
