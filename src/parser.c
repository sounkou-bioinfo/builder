#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

void get_arg_value(char **value, int argc, char *argv[], char *arg)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], arg) == 0)
    {
      *value = malloc(sizeof(char) * strlen(argv[i + 1]));
      strcpy(*value, argv[i + 1]);
      return;
    }
  }
  return;
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
    if (strcmp(argv[i], "-noclean") == 1)
    {
      continue;
    }

    if (strcmp(argv[i], "-input") == 1)
    {
      i++;
      continue;
    }

    if(strcmp(argv[i], "-output") == 1)
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
