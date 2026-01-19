#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Rinternals.h>
#include <R_ext/Parse.h>

#include "log.h"
#include "parser.h"
#include "define.h"

static char *get_path_from_package(char *input)
{
  char *delimiter = "::";
  char *split_point = strstr(input, delimiter);
  size_t name_len = split_point - input;
          
  char *name = malloc(name_len + 1);
  strncpy(name, input, name_len);
  name[name_len] = '\0';

  char *path = strdup(split_point + strlen(delimiter));

  SEXP system_file = PROTECT(install("system.file"));

  SEXP filename = PROTECT(mkString(path));
  SEXP pkg_name = PROTECT(mkString(name));

  free(name);
  free(path);

  SEXP call = PROTECT(lang3(system_file, filename, pkg_name));

  // name argument
  // R is fucking weird
  SET_TAG(CDDR(call), install("package"));

  SEXP result = PROTECT(eval(call, R_GlobalEnv));

  UNPROTECT(4);

  const char *filepath = CHAR(asChar(result));

  return strdup(filepath);
}

static char *get_path(char *path)
{
  if(strstr(path, "::") != NULL) {
    return get_path_from_package(path);
  }

  return strdup(path);
}

static char *get_namespace(char *path)
{
  if(strstr(path, "::") != NULL) {
    char *token = strtok(path, ":");
    return token;
  }

  return NULL;
}

void import_defines(Define **defines, Value *paths)
{
  if(paths == NULL) {
    return;
  }

  char line[1024];

  Value *current = paths;
  while(current != NULL) {
    char *path = get_path(current->name);
    char *namespace = get_namespace(current->name);
    FILE *file = fopen(path, "r");
    free(path);
    while(fgets(line, sizeof(line), file) != NULL) {
      if(define(defines, line, namespace)) {
        ingest_macro(defines, file, sizeof(line), namespace);
      }
    }
    fclose(file);
    current = current->next;
  }
}

int import_defines_from_line(Define **defines, char *line)
{
  char *import = strstr(line, "#import ");
  if(import == NULL) {
    return 0;
  }

  import += 8;

  size_t len = strlen(import);
  if(len > 0 && import[len - 1] == '\n') {
    import[len - 1] = '\0';
  }

  char *path = get_path(import);
  char *namespace = get_namespace(import);
  FILE *file = fopen(path, "r");
  free(path);

  char ln[1024];

  while(fgets(ln, sizeof(ln), file) != NULL) {
    if(define(defines, ln, namespace)) {
      ingest_macro(defines, file, sizeof(ln), namespace);
    }
  }
  fclose(file);

  return 1;
}
