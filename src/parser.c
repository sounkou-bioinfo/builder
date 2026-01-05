#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "define.h"

const char *NO_DEFINITION = "<UNDEFINED>";

int include(char *arg, char value)
{
  const int l = strlen(arg);
  for (int i = 0; i < l; i++)
  {
    if (arg[i] == value)
    {
      return 1;
    }
  }

  return 0;
}

int is_arg_name(char *arg) {
  if(strlen(arg) < 1) {
    return 0;
  }

  if(arg[0] == '-') {
    return 1;
  }

  return 0;
}

int is_directive(char *arg) {
  if(strlen(arg) < 2) {
    return 0;
  }

  if(arg[0] == '-' && arg[1] == 'D') {
    return 1;
  }
  return 0;
}

char *get_arg_value(int argc, char *argv[], char *arg)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], arg) == 0)
    {
      if(i + 1 >= argc) {
        return NULL;
      }
      return strdup(argv[i + 1]);
    }
  }
  return NULL;
}

int has_arg(int argc, char *argv[], char *arg)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], arg) == 0)
    {
      return 1;
    }
  }
  return 0;
}

void get_definitions(Define *arr, int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-noclean") == 0)
    {
      continue;
    }

    if (strcmp(argv[i], "-input") == 0)
    {
      i++;
      continue;
    }

    if(strcmp(argv[i], "-output") == 0)
    {
      i++;
      continue;
    }

    // these are directives
    if(is_directive(argv[i])) {
      if(i < argc - 1 && !is_arg_name(argv[i + 1])) {
        char *name = strdup(argv[i] + 2);
        char *value = strdup(argv[i + 1]);
        if(name == NULL || value == NULL) {
          free(name);
          free(value);
          return;
        }
        push(arr, name, value, DEF_VARIABLE);
        i++;
        continue;
      }

      char *name = strdup(argv[i] + 2);
      char *undefined = strdup(NO_DEFINITION);
      if(name == NULL || undefined == NULL) {
        free(name);
        free(undefined);
        return;
      }
      push(arr, name, undefined, DEF_VARIABLE);
    }
  }
}
