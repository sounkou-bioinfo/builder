#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugins.h"
#include "log.h"
#include "r.h"

Plugins *create_plugins(char *name, int setup, SEXP obj)
{
  Plugins *plugins = malloc(sizeof(Plugins));
  if(plugins == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return NULL;
  }

  plugins->name = strdup(name);
  plugins->setup = setup;
  plugins->obj = obj;
  plugins->next = NULL;

  return plugins;
}

Plugins *push_plugins(Plugins *head, char *name, int setup, SEXP obj)
{
  if(head == NULL) {
    return create_plugins(name, setup, obj);
  }

  Plugins *new = create_plugins(name, setup, obj);
  if(new == NULL) {
    return NULL;
  }

  Plugins *current = head;
  while(current->next != NULL) {
    current = current->next;
  }

  current->next = new;
  return head;
}

Plugins *plugins_init(Value *plugins, char *input, char *output)
{
  Plugins *head = NULL;

  Value *current = plugins;
  while(current != NULL) {
    char *copy = strdup(current->name);

    char *pkg = strtok(copy, ":");
    char *fn = strtok(NULL, ":");

    SEXP ns = PROTECT(R_FindNamespace(mkString(pkg)));
    SEXP func = PROTECT(findVarInFrame(ns, install(fn)));

    free(copy);

    SEXP fn_call = PROTECT(lang1(func));
    SEXP result = PROTECT(eval(fn_call, R_GlobalEnv));
    UNPROTECT(2);

    defineVar(install(current->name), result, R_GlobalEnv);

    if(result == NULL) {
      printf("%s Failed to initialize plugin: %s\n", LOG_ERROR, current->name);
      head = push_plugins(head, current->name, 0, R_NilValue);
      current = current->next;
      continue;
    }

    SEXP obj = PROTECT(findVar(install(current->name), R_GlobalEnv));

    SEXP setup_func = PROTECT(
      eval(
        lang3(install("$"), obj, install("setup")), R_GlobalEnv
      )
    );

    SEXP setup_call = PROTECT(lang3(setup_func, mkString(input), mkString(output)));
    result = PROTECT(eval(setup_call, R_GlobalEnv));
    UNPROTECT(3);

    if(result == NULL) {
      printf("%s Failed to initialize plugin: %s\n", LOG_ERROR, current->name);
      head = push_plugins(head, current->name, 0, R_NilValue);
      current = current->next;
      continue;
    }

    head = push_plugins(head, current->name, 1, obj);

    printf("%s Initialized plugin: %s\n", LOG_INFO, current->name);

    current = current->next;
  }

  return head;
}

int plugins_failed(Plugins *head)
{
  Plugins *current = head;
  while(current != NULL) {
    if(current->setup == 0) {
      return 1;
    }
    current = current->next;
  }
  return 0;
}

void plugins_call(Plugins *head, char *fn, char *str)
{
  Plugins *current = head;
  while(current != NULL) {
    current = current->next;
  }
}
