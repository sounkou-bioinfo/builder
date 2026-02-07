#include <stdio.h>
#include <string.h>

#include "preflight.h"

int starts_preflight(char *line)
{
  return strncmp(line, "#> preflight", 12) == 0;
}

int ends_preflight(char *line)
{
  if(strncmp(line, "#> endpreflight", 15) == 0) return 1;
  return strncmp(line, "#> endflight", 12) == 0;
}
