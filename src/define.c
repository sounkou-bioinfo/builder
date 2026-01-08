#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/utsname.h>

#include "define.h"
#include "parser.h"
#include "log.h"
#include "r.h"

const char *DYNAMIC_DEFINITION = "<DYNAMIC>";

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

  arr->type = (DefineType*)malloc(arr->capacity * sizeof(DefineType));
  if(arr->type == NULL) {
    free(arr->name);
    free(arr->value);
    free(arr);
    return NULL;
  }

  push_builtins(arr);

  return arr;
}

void overwrite(Define **arr, char *name, char *value)
{
  for(int i = 0; i < (*arr)->size; i++) {
    if(strcmp((*arr)->name[i], name) == 0) {
      free((*arr)->value[i]);
      (*arr)->value[i] = strdup(value);
      return;
    }
  }
}

void push_builtins(Define *arr)
{
  push(arr, strdup("__FILE__"), strdup(DYNAMIC_DEFINITION), DEF_VARIABLE);
  push(arr, strdup("__LINE__"), strdup(DYNAMIC_DEFINITION), DEF_VARIABLE);

  struct utsname buffer;

  if(uname(&buffer) == -1) {
    return;
  }

  push(arr, strdup("__OS__"), strdup(buffer.sysname), DEF_VARIABLE);
}

void push(Define *arr, char *name, char *value, DefineType type)
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

    DefineType *tmp_type = realloc(arr->type, arr->capacity * sizeof(DefineType));
    if(tmp_type == NULL) {
      arr->capacity -= 8;
      return;
    }
    arr->type = tmp_type;
  }

  arr->name[arr->size] = name;
  arr->value[arr->size] = value;
  arr->type[arr->size] = type;
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
  free(arr->type);

  free(arr);
}

int is_macro(char *name)
{
  for(int i = 0; i < strlen(name); i++) {
    if(name[i] == '(') {
      return 1;
    }
  }

  return 0;
}

void *define_macro_init(char **macro, char *line)
{
  *macro = malloc(strlen(line + 8) + 1);
  if(*macro == NULL) {
      return NULL;
  }
  strcpy(*macro, line + 8);
  return *macro;
}

char *define_macro(char *line)
{
  char *p = strstr(line, "#define");
  if(!p) {
    return p;
  }

  p+=8;
  const char *start = p;

  while (*p && *p != '(') {
    p++;
  }

  int len = p - start;
  return strndup(start, len);
}

int define(Define **defines, char *line)
{
  if(strncmp(line, "#define", 7) != 0) {
    return 0;
  }

  char *token;
  char *copy = strdup(line);
  if(copy == NULL) {
    return 0;
  }

  // Skip "#define"
  token = strtok(copy, " ");
  token = strtok(NULL, " ");
  if(token == NULL) {
    free(copy);
    return 0;
  }

  char *name_copy = strdup(token);
  if(name_copy == NULL) {
    free(copy);
    return 0;
  }

  if(is_macro(name_copy)) {
    // it's ugly but will do for now
    // we just flag upstream that it's a function
    // we do so because it may be multiline
    free(name_copy);
    free(copy);
    return 1;
  }

  // check if already defined
  // CLI overrides defines
  if(get_define_value(defines, name_copy) != NULL) {
    printf("%s %s is already defined by the command line\n", name_copy, LOG_WARNING);
    free(name_copy);
    free(copy);
    return 0;
  }

  // Get value - capture everything after the name
  char *value_copy = NULL;
  char *rest = strtok(NULL, "");
  if(rest != NULL) {
    while(*rest == ' ' || *rest == '\t') {
      rest++;
    }
    if(*rest != '\0') {
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
          return 0;
        }
      }
    }
  }

  push(*defines, name_copy, value_copy, DEF_VARIABLE);
  free(copy);

  return 0;
}

char* str_replace(const char *orig, const char *find, const char *replace) 
{
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

char* extract_first_line(const char *str, char *buffer, size_t buffer_size) 
{
  if (str == NULL || buffer == NULL || buffer_size == 0) {
    return NULL;
  }
  
  char *newline_pos = strchr(str, '\n');
  size_t length;
  
  if (newline_pos != NULL) {
    length = newline_pos - str;
  } else {
    length = strlen(str);
  }
  
  if (length >= buffer_size) {
    length = buffer_size - 1;
  }
  
  strncpy(buffer, str, length);
  buffer[length] = '\0';
  
  return buffer;
}

char *define_replace(Define **defines, char *line)
{
  if(*defines == NULL) {
    return strdup(line);
  }

  if(strncmp(line, "#ifdef", 6) == 0) {
    return strdup(line);
  }

  if(strncmp(line, "#ifndef", 6) == 0) {
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
    char type = (*defines)->type[i];

    if(name == NULL || value == NULL || strcmp(value, NO_DEFINITION) == 0) {
      continue;
    }

    if(type == DEF_VARIABLE) {
      char *replaced = str_replace(current, name, value);
      if(replaced == NULL) {
        continue;
      }

      free(current);
      current = replaced;
      continue;
    }

    if(strstr(line, name) == NULL) {
      continue;
    }

    // if not a var it's a function call
    // or is it?
    // could be used in #include:MACRO
    char *call = strdup(name);
    strcat(call, "(");
    if(strstr(line, call) == NULL) {
      free(call);
      continue;
    }

    free(call);

    // we cleanup the function name
    char fn[1028];
    extract_first_line(value, fn, sizeof(fn));
    int nargs_macro;
    // we extract the macro arguments
    char **args_macro = extract_macro_args(fn, &nargs_macro);

    char *body_macro = extract_function_body(value);

    // we extract the function arguments
    int nargs;
    char **args = extract_macro_args(current, &nargs);

    if(nargs != nargs_macro) {
      printf("%s Macro %s has %d arguments but function %s has %d arguments\n", LOG_ERROR, name, nargs_macro, fn, nargs);
      return current;
    }

    for(int i = 0; i < nargs; i++) {
      char *old_body = body_macro;
      body_macro = str_replace(body_macro, args_macro[i], args[i]);
      free(old_body);
    }

    // Free args_macro array and individual strings
    for(int i = 0; i < nargs_macro; i++) {
      free(args_macro[i]);
    }
    free(args_macro);

    // Free args array and individual strings
    for(int i = 0; i < nargs; i++) {
      free(args[i]);
    }
    free(args);

    // Free current since we're returning body_macro instead
    free(current);

    return body_macro;
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
