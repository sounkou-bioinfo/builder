#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "define.h"
#include "parser.h"

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
    if(arr->name[i] != NULL) {
      free(arr->name[i]);
    }
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

  // checkjs if already defined
  // CLI overrides defines
  if(get_define_value(defines, name_copy) != NULL) {
    printf("[WARNING] %s is already defined by the command line\n", name_copy);
    free(name_copy);
    free(copy);
    return;
  }

  // Get value - capture everything after the name
  char *value_copy = NULL;
  char *rest = strtok(NULL, "");  // Get rest of line
  if(rest != NULL) {
    // Skip leading whitespace
    while(*rest == ' ' || *rest == '\t') {
      rest++;
    }
    if(*rest != '\0') {
      // Remove trailing newline/carriage return
      int len = strlen(rest);
      while(len > 0 && (rest[len-1] == '\n' || rest[len-1] == '\r')) {
        rest[len-1] = '\0';
        len--;
      }

      if(*rest != '\0') {
        value_copy = strdup(rest);
        if(value_copy == NULL) {
          free(name_copy);
          free(copy);
          return;
        }
      }
    }
  }

  push(*defines, name_copy, value_copy);
  free(copy);
}

char* str_replace(const char *orig, const char *find, const char *replace) {
  const char *pos = orig;
  int count = 0;
  int find_len = strlen(find);
  int replace_len = strlen(replace);
  
  while ((pos = strstr(pos, find)) != NULL) {
    count++;
    pos += find_len;
  }
  
  if (count == 0) {
    return strdup(orig);
  }
  
  int orig_len = strlen(orig);
  int new_len = orig_len + count * (replace_len - find_len);
  
  char *result = malloc(new_len + 1);
  if (!result) {
    return NULL;
  }
  
  char *current = result;
  const char *src = orig;
  
  while ((pos = strstr(src, find)) != NULL) {
    int len_before = pos - src;
    memcpy(current, src, len_before);
    current += len_before;
    
    memcpy(current, replace, replace_len);
    current += replace_len;
    
    src = pos + find_len;
  }
  
  strcpy(current, src);
  
  return result;
}

char *define_replace(Define **defines, char *line)
{
  if(*defines == NULL) {
    return strdup(line);
  }

  if(strncmp(line, "#define", 7) == 0) {
    return strdup(line);
  }

  char *current = strdup(line);
  if(current == NULL) {
    return NULL;
  }

  for(int i = 0; i < (*defines)->size; i++) {
    char *name = (*defines)->name[i];
    char *value = (*defines)->value[i];

    if(name == NULL || value == NULL || strcmp(value, NO_DEFINITION) == 0) {
      continue;
    }

    char *replaced = str_replace(current, name, value);
    if(replaced == NULL) {
      continue;
    }

    free(current);
    current = replaced;
  }

  return current;
}

char *get_define_value(Define **defines, char *name)
{
  if(defines == NULL) {
    return NULL;
  }

  for(int i = 0; i < (*defines)->size; i++) {
    if(strcmp((*defines)->name[i], name) == 0) {
      return (*defines)->value[i];
    }
  }

  return NULL;
}

void print_defines(Define *defines)
{
  if(defines == NULL) {
    return;
  }

  for(int i = 0; i < defines->size; i++) {
    printf("%s = %s\n", defines->name[i], defines->value[i]);
  }
}
