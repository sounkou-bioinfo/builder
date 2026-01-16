#ifndef PARSER_H
#define PARSER_H

#include "define.h"
 
struct Value_t {
  char *name;
  struct Value_t *next;
};

typedef struct Value_t Value;

extern const char *NO_DEFINITION;

char *get_arg_value(int argc, char *argv[], char *arg);
int has_arg(int argc, char *argv[], char *arg);
void get_definitions(Define *arr, int argc, char *argv[]);
Value *get_arg_values(int argc, char *argv[], char *arg);

#endif
