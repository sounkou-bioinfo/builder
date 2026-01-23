#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"

int catch_error(char* line)
{
  if(strstr(line, "#error") == NULL) return 0;

  while(line[0] == ' ' || line[0] == '\t') line++;

  char *match = strstr(line, "#error ");

  char *msg = match + strlen("#error ");

  printf("%s Error: %s\n", LOG_ERROR, msg);

  return 1;
}
