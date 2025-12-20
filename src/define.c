#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "define.h"

Define *create_define()
{
  Define *arr = malloc(sizeof(Define));
  if(arr == NULL) {
    return NULL;
  }

  arr->capacity = 8;
  arr->size = 0;

  arr->name = malloc(arr->capacity * sizeof(char*));
  if(arr->name == NULL) {
    free(arr);
    return NULL;
  }

  arr->value = malloc(arr->capacity * sizeof(char*));
  if(arr->value == NULL) {
    free(arr->name);
    free(arr);
    return NULL;
  }

  return arr;
}

void push(Define *arr, char *name, char *value)
{
  if(arr->size == arr->capacity) {
    arr->capacity += 8;

    char **tmp_name = realloc(arr->name, arr->capacity * sizeof(char*));
    if(tmp_name == NULL) {
      arr->capacity -= 8;
      return;
    }
    arr->name = tmp_name;

    char **tmp_value = realloc(arr->value, arr->capacity * sizeof(char*));
    if(tmp_value == NULL) {
      arr->capacity -= 8;
      return;
    }
    arr->value = tmp_value;
  }

  arr->name[arr->size] = name;
  arr->value[arr->size] = value;
  arr->size++;
}

void free_array(Define *arr)
{
  if(arr == NULL) {
    return;
  }

  for(int i = 0; i < arr->size; i++) {
    free(arr->name[i]);
    if(arr->value[i] != NULL) {
      free(arr->value[i]);
    }
  }

  free(arr->name);
  free(arr->value);

  free(arr);
}

void define(Define **defines, char *line)
{
  if(strncmp(line, "#define", 7) != 0) {
    return;
  }

  char *token;
  char *copy = strdup(line);
  if(copy == NULL) {
    return;
  }
    
  // Skip "#define"
  token = strtok(copy, " ");
  token = strtok(NULL, " ");
  if(token == NULL) {
    free(copy);
    return;
  }

  char *name_copy = strdup(token);
  if(name_copy == NULL) {
    free(copy);
    return;
  }

  // Get value
  token = strtok(NULL, " ");
  char *value_copy = NULL;
  if(token != NULL) {
    value_copy = strdup(token);
    if(value_copy == NULL) {
      free(name_copy);
      free(copy);
      return;
    }
  }

  push(*defines, name_copy, value_copy);
  free(copy);
}
