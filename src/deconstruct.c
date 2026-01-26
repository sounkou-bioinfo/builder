#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deconstruct.h"
#include "file.h"

static Var *create_var(char *value)
{
  Var *f = (Var*)malloc(sizeof(Var));
  if(f == NULL) {
    return NULL;
  }

  f->value = (char*)malloc(strlen(value) + 1);
  strcpy(f->value, value);
  f->next = NULL;

  return f;
}

static void push_var(Var **head, char *value)
{
  Var *new = create_var(value);

  // create if it doesn't exist
  if(*head == NULL) {
    *head = new;
    return;
  }

  Var *curr = *head;
  while(curr->next != NULL) {
    curr = curr->next;
  }

  curr->next = new;
}

static void free_var(Var *f)
{
  if(f == NULL) {
    return;
  }

  Var *next = f->next;
  free(f->value);
  free(f);
  free_var(next);  // Recursively free the rest
}


char *deconstruct_replace(char *line)
{
  if(line == NULL) {
    return NULL;
  }

  char *trimmed = remove_leading_spaces(line);
  if(trimmed[0] != '.') {
    return line;
  }

  Var *vars = NULL;

  int deconstruct = 0;
  char buffer[256];
  int buffer_len = 0;
  for(int i = 0; i < strlen(line); i++)
  {
    if(strlen(line) > i + 1 && line[i] == '.' && line[i+1] == '['){
      deconstruct = 1;
      i++;
      continue;
    }

    if(line[i] == ' ') continue;

    if(!deconstruct) continue;

    if(deconstruct && line[i] != ',' && line[i] != ']') {
      buffer[buffer_len] = line[i];
      buffer_len++;
    }

    if(deconstruct && (line[i] == ']' || line[i] == ',')) {
      buffer[buffer_len] = '\0';
      buffer_len = 0;
      push_var(&vars, buffer);
      continue;
    }

    if(deconstruct && line[i] == ']') {
      deconstruct = 0;
      break;
    }
  }

  if(vars == NULL) {
    return line;
  }

  char *subbed = strstr(line, " <-");
  if(subbed[strlen(subbed) - 1] == '\n') { 
    subbed[strlen(subbed) - 1] = '\0';
  }
  char new[1024];
  char str[32];

  // First: assign RHS to temp variable (evaluated once)
  strcpy(new, ".destructure_tmp_");
  strcat(new, subbed);

  // Then: extract each element from the temp variable
  int i = 1;
  Var *current = vars;
  while(current != NULL) {
    strcat(new, "\n");
    strcat(new, current->value);
    strcat(new, " <- .destructure_tmp_[[");
    sprintf(str, "%d", i);
    strcat(new, str);
    strcat(new, "]]");
    current = current->next;
    i++;
  }

  char *result = strdup(new);
  free_var(vars);
  return result;
}
