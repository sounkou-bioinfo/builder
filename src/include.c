#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "define.h"

void free_include(Include *include)
{
  if(include->function != NULL) free(include->function);
  if(include->path != NULL) free(include->path);
  if(include->object != NULL) free(include->object);
}

Include parse_include(char *line)
{
  Include result = {NULL, NULL, NULL};

  const char *start = strstr(line, "#include:");
  if(start == NULL) return result;

  start += strlen("#include:");

  char *work = strdup(start);
  if(work == NULL) return result;

  char *saveptr;
  char *token;
  int part = 0;

  token = strtok_r(work, " ", &saveptr);
  while(token != NULL && part < 3) {
    switch (part) {
      case 0:
        result.function = strdup(token); break;
      case 1:
        result.path = strdup(token); break;
      case 2:
        result.object = strdup(token); break;
    }
    part++;
    token = strtok_r(NULL, " ", &saveptr);
  }

  free(work);
  return result;
}

char *include_replace(Define **defs, char *line)
{
  Include inc = parse_include(line);

  // nothing to do
  if(inc.function == NULL) {
    free_include(&inc);
    return line;
  }

  char *def = get_define_value(defs, inc.function);

  if(def == NULL) {
    printf("Failed to find definition for %s\n", inc.function);
    free_include(&inc);
    return line;
  } 

  printf("Replacing include %s with %s\n", inc.function, def);

  return 0;
}
