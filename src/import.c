#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Rinternals.h>
#include <R_ext/Parse.h>

#include "log.h"
#include "parser.h"
#include "define.h"

char *get_path_from_package(char *input)
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

  SEXP call = PROTECT(lang3(system_file, filename, pkg_name));

  // name argument
  // R is fucking weird
  SET_TAG(CDDR(call), install("package"));

  SEXP result = PROTECT(eval(call, R_GlobalEnv));

  UNPROTECT(4);

  const char *filepath = CHAR(asChar(result));

  return strdup(filepath);
}

char *get_path(char *path)
{
  if(strstr(path, "::") != NULL) {
    return get_path_from_package(path);
  }

  return strdup(path);
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
    FILE *file = fopen(path, "r");
    free(path);
    while(fgets(line, sizeof(line), file) != NULL) {
      define(defines, line);
    }
    fclose(file);
    current = current->next;
  }
}
