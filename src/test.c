#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

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

  if(expressions == NULL) {
    test->expressions = (char *)malloc(1);
    test->expressions[0] = '\0';
  } else {
    test->expressions = (char *)malloc(strlen(expressions) + 1);
    strcpy(test->expressions, expressions);
  }

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

static int create_test_directory(const char *path)
{
  // Create tests/ directory
  if(mkdir("tests", 0755) == -1 && errno != EEXIST) {
    printf("%s Failed to create tests/ directory: %s\n", LOG_ERROR, strerror(errno));
    return 1;
  }

  // Create tests/testthat/ directory
  if(mkdir("tests/testthat", 0755) == -1 && errno != EEXIST) {
    printf("%s Failed to create tests/testthat/ directory: %s\n", LOG_ERROR, strerror(errno));
    return 1;
  }

  return 0;
}

static char *make_test_filename(const char *src)
{
  const char *filename = strrchr(src, '/');
  if(filename == NULL) {
    filename = src;
  } else {
    filename++; 
  }

  const char *first_slash = strchr(src, '/');
  char *subdir_part = NULL;

  if(first_slash != NULL && filename != first_slash + 1) {
    size_t subdir_len = filename - first_slash - 1;
    subdir_part = malloc(subdir_len + 2);
    strncpy(subdir_part, first_slash + 1, subdir_len);
    subdir_part[subdir_len] = '-';
    subdir_part[subdir_len + 1] = '\0';

    for(int i = 0; i < subdir_len; i++) {
      if(subdir_part[i] == '/') {
        subdir_part[i] = '-';
      }
    }
  }

  size_t len = strlen("tests/testthat/test-builder-") +
               (subdir_part ? strlen(subdir_part) : 0) +
               strlen(filename) + 1;
  char *test_path = malloc(len);

  if(subdir_part) {
    snprintf(test_path, len, "tests/testthat/test-builder-%s%s", subdir_part, filename);
    free(subdir_part);
  } else {
    snprintf(test_path, len, "tests/testthat/test-builder-%s", filename);
  }

  return test_path;
}

void write_tests(Tests *tests, const char *src)
{
  if(tests == NULL) {
    return;  // No tests to write
  }

  // Create test directory structure
  if(create_test_directory("tests/testthat") != 0) {
    return;
  }

  // Generate test filename
  char *test_filename = make_test_filename(src);

  FILE *f = fopen(test_filename, "w");
  if(f == NULL) {
    printf("%s Failed to create test file: %s\n", LOG_ERROR, test_filename);
    free(test_filename);
    return;
  }

  printf("%s Writing tests to %s\n", LOG_INFO, test_filename);

  int tests_written = 0;

  Tests *current = tests;
  while(current != NULL) {
    if(current->expressions == NULL || strlen(current->expressions) == 0) {
      printf("%s Skipping empty test: \"%s\"\n", LOG_WARNING, current->description);
      Tests *next = current->next;
      free_test(current);
      current = next;
      continue;
    }

    fprintf(f, "test_that(\"%s\", {\n", current->description);
    fprintf(f, "%s", current->expressions);
    fprintf(f, "})\n\n");

    tests_written++;

    Tests *next = current->next;
    free_test(current);
    current = next;
  }

  fclose(f);

  if(tests_written == 0) {
    printf("%s No valid tests to write, removing %s\n", LOG_WARNING, test_filename);
    remove(test_filename);
  }

  free(test_filename);
}
