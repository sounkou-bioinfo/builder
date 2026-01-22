#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h> 
#include <sys/utsname.h>

#include "define.h"
#include "parser.h"
#include "log.h"
#include "r.h"

static const int MAX_MACRO_DEPTH = 32;

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
  time_t now = time(NULL);
  struct tm *local = localtime(&now);
  char date[11];
  char time_str[9];
  strftime(date, sizeof(date), "%Y-%m-%d", local);
  strftime(time_str, sizeof(time_str), "%H:%M:%S", local);

  push(arr, strdup("__FILE__"), strdup(DYNAMIC_DEFINITION), DEF_VARIABLE);
  push(arr, strdup("__LINE__"), strdup(DYNAMIC_DEFINITION), DEF_VARIABLE);
  push(arr, strdup("__COUNTER__"), strdup("-1"), DEF_VARIABLE);
  push(arr, strdup("__DATE__"), strdup(date), DEF_VARIABLE);
  push(arr, strdup("__TIME__"), strdup(time_str), DEF_VARIABLE);

  struct utsname buffer;

  if(uname(&buffer) == -1) {
    return;
  }

  push(arr, strdup("__OS__"), strdup(buffer.sysname), DEF_VARIABLE);
}

void increment_counter(Define **arr, char *line)
{
  if(arr == NULL || line == NULL) {
    return;
  }

  if(strstr(line, "__COUNTER__") == NULL) {
    return;
  }

  char *value = get_define_value(arr, "__COUNTER__");

  if(value == NULL) {
    return;
  }

  int counter = atoi(value);
  counter++;
  asprintf(&value, "%d", counter);
  overwrite(arr, "__COUNTER__", value);
  free(value);

  return;
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

void *define_macro_init(char **macro)
{
  *macro = strdup("");
  return *macro;
}

int ingest_macro(Define **defs, FILE *src_file, size_t line_len, char *ns)
{
  char *macro;
  define_macro_init(&macro);

  char line[line_len];
  char *macro_name = NULL;
  int macro_lines = 0;
  int found_signature = 0;
  int max_macro_lines = 1024;

  while(fgets(line, line_len, src_file) != NULL && macro_lines < max_macro_lines) {
    macro_lines++;

    if(strncmp(line, "#enddef", 7) == 0) {
      break;
    }

    if(!found_signature) {
      char *trimmed = line;
      while(*trimmed == ' ' || *trimmed == '\t') trimmed++;
      if(*trimmed != '\0' && *trimmed != '\n') {
        char *paren = strchr(trimmed, '(');
        if(paren) {
          macro_name = strndup(trimmed, paren - trimmed);
        }
        found_signature = 1;
      }
    }

    size_t l = strlen(macro) + strlen(line) + 1;
    macro = realloc(macro, l);
    strcat(macro, line);
  }

  if(macro_lines == max_macro_lines) {
    printf("%s macro %s is too long (or failed to parse), reached %d lines\n", LOG_WARNING, macro_name ? macro_name : "<unknown>", macro_lines);
    free(macro);
    if(macro_name) free(macro_name);
    return 1;
  }

  if(ns != NULL) {
    char *ccopy = strdup(macro_name);
    macro_name = realloc(macro_name, strlen(macro_name) + strlen(ns) + 3);
    strcpy(macro_name, ns);
    strcat(macro_name, "::");
    strcat(macro_name, ccopy);
    free(ccopy);
  }

  push((*defs), macro_name, macro, DEF_FUNCTION);
  return 0;
}

void push_macro(Define **defs, char *macro, char *ns)
{
  char *paren = strchr(macro, '(');
  if(paren == NULL) {
    return;
  }

  size_t len = paren - macro;
  char* name = malloc(len + 1);
  strncpy(name, macro, len);
  name[len] = '\0';

  if(ns != NULL) {
    char *prefixed = malloc(strlen(ns) + 2 + strlen(name) + 1);
    sprintf(prefixed, "%s::%s", ns, name);
    free(name);
    name = prefixed;
  }

  push((*defs), strdup(name), macro, DEF_FUNCTION);
  free(name);
}

int define(Define **defines, char *line, char *ns)
{
  if(strncmp(line, "#define", 7) != 0) {
    return 0;
  }

  char *check = line + 7;
  while(*check == ' ' || *check == '\t') check++;
  if(*check == '\n' || *check == '\r' || *check == '\0') {
    return 1;
  }

  char *token;
  char *copy = strdup(line);
  if(copy == NULL) {
    return 0;
  }

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

  if(get_define_value(defines, name_copy) != NULL) {
    printf("%s %s is already defined\n", LOG_WARNING, name_copy);
    free(name_copy);
    free(copy);
    return 0;
  }

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

  if(ns != NULL) {
    char *ccopy = strdup(name_copy);
    name_copy = realloc(name_copy, strlen(name_copy) + strlen(ns) + 3);
    strcpy(name_copy, ns);
    strcat(name_copy, "::");
    strcat(name_copy, ccopy);
    free(ccopy);
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
  if(find_len == 0) {
    return strdup(orig);
  }
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

static char* extract_first_line(const char *str, char *buffer, size_t buffer_size)
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

static char *define_replace_once(Define **defines, char *line)
{
  // we define, nothing to do
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
    char *call = malloc(strlen(name) + 2);  // +1 for '(', +1 for '\0'
    strcpy(call, name);
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
      // Cleanup before returning
      free(body_macro);
      for(int j = 0; j < nargs_macro; j++) {
        free(args_macro[j]);
      }
      free(args_macro);
      for(int j = 0; j < nargs; j++) {
        free(args[j]);
      }
      free(args);
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

char *define_replace(Define **defines, char *line)
{
  if (*defines == NULL) {
    return strdup(line);
  }

  char *current = strdup(line);
  if (current == NULL) {
    return NULL;
  }

  int depth = 0;

  while (depth < MAX_MACRO_DEPTH) {
    char *result = define_replace_once(defines, current);
    if (result == NULL) {
      return current;
    }

    // Nothing changed, we're done
    if (strcmp(result, current) == 0) {
      free(result);
      break;
    }

    free(current);
    current = result;
    depth++;
  }

  if (depth == MAX_MACRO_DEPTH) {
    printf(
      "%s Max macro expansion depth (%d) reached, possible circular definition\n",
      LOG_WARNING, MAX_MACRO_DEPTH
    );
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
