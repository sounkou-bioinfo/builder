#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "test.h"
#include "file.h"

Tests *create_test(char *description, char *expressions)
{
  Tests *test = malloc(sizeof(Tests));
  if(test == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return NULL;
  }

  test->description = (char *)malloc(strlen(description) + 1);
  strcpy(test->description, description);

  test->expressions = (char *)malloc(strlen(expressions) + 1);
  strcpy(test->expressions, expressions);

  test->next = NULL;

  return test;
}

void push_test(Tests **tests, Tests *test)
{
  if(*tests == NULL) {
    *tests = test;
    return;
  }

  Tests *t = *tests;
  while(t->next != NULL) {
    t = t->next;
  }
  t->next = test;
}

void free_test(Tests *test)
{
  free(test->description);
  free(test->expressions);
  free(test);
}

int enter_test(char *line)
{
  char *trimmed = remove_leading_spaces(line);
  return strncmp(trimmed, "#test ", 6) == 0;
}
