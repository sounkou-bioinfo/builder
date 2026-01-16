#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugins.h"
#include "log.h"
#include "r.h"

Plugins *create_plugins(char *name, int setup)
{
  Plugins *plugins = malloc(sizeof(Plugins));
  if(plugins == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return NULL;
  }

  plugins->name = strdup(name);
  plugins->setup = setup;
  plugins->next = NULL;

  return plugins;
}

Plugins *push_plugins(Plugins *head, char *name, int setup)
{
  if(head == NULL) {
    return create_plugins(name, setup);
  }

  Plugins *new = create_plugins(name, setup);
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

Plugins *plugins_init(Value *plugins)
{
  Plugins *head = NULL;

  Value *current = plugins;
  while(current != NULL) {
    // we just initi all in global env
    // `pkg::plugin`=pkg::plugin()
    char *call = (char *)malloc(strlen(current->name) + strlen("=") + strlen(current->name) + strlen("()") + 3);
    strcpy(call, "`");
    strcat(call, current->name);
    strcat(call, "`=");
    strcat(call, current->name);
    strcat(call, "()");

    SEXP result = evaluate(call);
    if(result == NULL) {
      free(call);
      printf("%s Failed to initialize plugin: %s\n", LOG_ERROR, current->name);
      head = push_plugins(head, current->name, 0);
      current = current->next;
      continue;
    }

    char *setup_call = (char *)malloc(strlen(current->name) + strlen("`$setup()") + 1);
    strcpy(setup_call, "`");
    strcat(setup_call, current->name);
    strcat(setup_call, "`$setup()");
    SEXP setup = evaluate(setup_call);

    if(setup == NULL) {
      printf("%s Failed to setup plugin: %s\n", LOG_ERROR, current->name);
      head = push_plugins(head, current->name, 0);
      current = current->next;
      free(setup_call);
      free(call);
      continue;
    }
    free(setup_call);
    free(call);

    head = push_plugins(head, current->name, 1);

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
