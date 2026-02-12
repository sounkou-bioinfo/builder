#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r.h"
#include "log.h"
#include "parser.h"

static int is_installed(char *package)
{
  char *call = (char*)malloc(strlen(package) + 37);
  snprintf(call, strlen(package) + 37, "requireNamespace('%s', quietly = TRUE)", package);
  int result = evaluate_if(call);
  free(call);
  return result;
}

int process_depends(Value *depends)
{
  int result = 0;
  Value *current = depends;
  while (current != NULL) {
    int installed = is_installed(current->name);
    if (!installed) {
      printf("%s Package '%s' is not installed\n", LOG_ERROR, current->name);
      result = 1;
    }
    current = current->next;
  }
  return result;
}
