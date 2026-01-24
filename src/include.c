#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "define.h"
#include "r.h"
#include "log.h"

static void free_include(Include *include)
{
  if(include->function != NULL) free(include->function);
  if(include->path != NULL) free(include->path);
  if(include->object != NULL) free(include->object);
}

static int has_include(char *line)
{
  return strstr(line, "#include:") != NULL;
}

static Include parse_include(char *line)
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

  // Extract argument name from macro signature (e.g., "READ(FILE)" -> "FILE")
  char *paren_start = strchr(def, '(');
  char *paren_end = strchr(def, ')');
  if(paren_start == NULL || paren_end == NULL || paren_end <= paren_start) {
    printf("%s Invalid macro signature for %s\n", LOG_ERROR, inc.function);
    free_include(&inc);
    return line;
  }
  
  size_t arg_len = paren_end - paren_start - 1;
  char *arg_name = malloc(arg_len + 1);
  strncpy(arg_name, paren_start + 1, arg_len);
  arg_name[arg_len] = '\0';
  
  // Get the macro body and apply .arg / ..arg replacements
  char *body = strdup(def);
  
  // Replace ..argname -> "value" (stringify) - MUST be first
  char *dblpat = NULL;
  asprintf(&dblpat, "..%s", arg_name);
  char *quoted = NULL;
  asprintf(&quoted, "\"%s\"", inc.path);
  char *tmp = str_replace(body, dblpat, quoted);
  free(body);
  body = tmp;
  free(dblpat);
  free(quoted);
  
  // Replace .argname -> 'value' (quoted, since it's a file path)
  char *dotpat = NULL;
  asprintf(&dotpat, ".%s", arg_name);
  char *quoted_path = NULL;
  asprintf(&quoted_path, "'%s'", inc.path);
  tmp = str_replace(body, dotpat, quoted_path);
  free(body);
  body = tmp;
  free(dotpat);
  free(quoted_path);
  free(arg_name);

  char *replaced = str_replace(body, inc.function, "(\\");
  free(body);

  size_t expr_size = strlen(replaced) + strlen(")() |> dput() |> capture.output()") + 1;
  char *expr = malloc(expr_size);
  if(expr == NULL) {
    printf("%s Failed to allocate memory for expression\n", LOG_ERROR);
    free(replaced);
    free_include(&inc);
    return line;
  }

  // Build the full expression (no argument passing needed now, path is already substituted)
  strcpy(expr, replaced);
  strcat(expr, ")() |> dput() |> capture.output()");
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
