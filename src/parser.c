#include <stdio.h>
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

char *get_arg_value(int argc, char *argv[], char *arg)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], arg) == 0)
    {
      return argv[i + 1];
    }
  }
  return "";
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
    if(include(argv[i], '-') && include(argv[i], 'D')) {
      buffer[j] = argv[i];
      j++;
    }
  }

  return j;
}
