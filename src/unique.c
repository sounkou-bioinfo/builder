#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "define.h"

int UNIQUE = 0;

void capture_unique_define(Define **defs, char *str)
{
  if(strstr(str, "#unique ") == NULL) return;

  char *var = strstr(str, "#unique ") + strlen("#unique ");

  char istr[2];
  sprintf(istr, "%d", UNIQUE);

  char *value = malloc(strlen("._unq.") + strlen(istr) + 1);
  strcpy(value, "._unq.");
  strcat(value, istr);

  push(*defs, strdup(var), strdup(value), DEF_VARIABLE);
  free(value);
  UNIQUE++;
}
