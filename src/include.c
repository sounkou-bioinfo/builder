#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "define.h"
#include "r.h"
#include "log.h"

void free_include(Include *include)
{
  if(include->function != NULL) free(include->function);
  if(include->path != NULL) free(include->path);
  if(include->object != NULL) free(include->object);
}

int has_include(char *line)
{
  return strstr(line, "#include:") != NULL;
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

  // remove line break
  int l = strlen(result.object);
  if(l > 0) {
    result.object[l - 1] = '\0';
  }

  free(work);
  return result;
}

char *include_replace(Define **defs, char *line, Plugins *plugins, char *file)
{
  if(!has_include(line)) {
    return line;
  }

  char *plugged = plugins_call(plugins, "include", line, file);
  if(strcmp(plugged, line) != 0) {
    free(line);
    return plugged;
  }
  free(plugged);

  Include inc = parse_include(line);

  // nothing to do
  if(inc.function == NULL) {
    free_include(&inc);
    return line;
  }

  char *def = get_define_value(defs, inc.function);

  if(def == NULL) {
    printf("%s Failed to find definition for %s\n", LOG_ERROR, inc.function);
    free_include(&inc);
    return line;
  }

  char *replaced = str_replace(def, inc.function, "(\\");

  size_t expr_size = strlen(replaced) + strlen(")('") + strlen(inc.path) + strlen("') |> dput() |> capture.output()") + 1;
  char *expr = malloc(expr_size);
  if(expr == NULL) {
    printf("%s Failed to allocate memory for expression\n", LOG_ERROR);
    free(replaced);
    free_include(&inc);
    return line;
  }

  // Build the full expression
  strcpy(expr, replaced);
  strcat(expr, ")('");
  strcat(expr, inc.path);
  strcat(expr, "') |> dput() |> capture.output()");
  free(replaced);

  const char *result = eval_string(expr);
  free(expr);

  if(result == NULL) {
    return line;
  }

  size_t result_size = strlen(inc.object) + strlen(" <- ") + strlen(result) + 1;
  char *result_copy = malloc(result_size);
  if(result_copy == NULL) {
    printf("%s Failed to allocate memory for result\n", LOG_ERROR);
    return line;
  }

  strcpy(result_copy, inc.object);
  strcat(result_copy, " <- ");
  strcat(result_copy, result);

  free_include(&inc);
  return result_copy;
}
