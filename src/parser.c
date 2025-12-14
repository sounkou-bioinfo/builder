#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int include(char *arg, char value)
{
  for (int i = 0; i < strlen(arg); i++)
  {
    if (arg[i] == value)
    {
      return 1;
    }
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
      char *value = malloc(strlen(argv[i + 1]) + 1);
      strcpy(value, argv[i + 1]);
      return value;
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

int get_extra_args(char **buffer, int argc, char *argv[])
{
  int j = 0;
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
      buffer[j] = argv[i];
      j++;
    }
  }

  return j;
}
