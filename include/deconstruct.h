#ifndef __DECONSTRUCT_H__
#define __DECONSTRUCT_H__

typedef struct Var_t {
  char *value;
  struct Var_t *next;
} Var;

char *deconstruct_replace(char *line);

#endif
