#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fstring.h"
#include "define.h"

Fstring *create_fstring(char *value)
{
  Fstring *f = (Fstring*)malloc(sizeof(Fstring));
  if(f == NULL) {
    return NULL;
  }

  f->value = (char*)malloc(strlen(value) + 1);
  strcpy(f->value, value);
  f->next = NULL;

  return f;
}

void push_fstring(Fstring **head, char *value)
{
  Fstring *new = create_fstring(value);

  // create if it doesn't exist
  if(*head == NULL) {
    *head = new;
    return;
  }

  Fstring *curr = *head;
  while(curr->next != NULL) {
    curr = curr->next;
  }

  curr->next = new;
}

void free_fstring(Fstring *f)
{
  if(f == NULL) {
    return;
  }

  Fstring *next = f->next;
  free(f->value);
  free(f);
  free_fstring(next);  // Recursively free the rest
}

// Find the closing quote, handling escaped quotes (\') and escaped backslashes (\\)
static char* find_closing_quote(const char *str)
{
  const char *p = str;
  int escaped = 0;

  while(*p != '\0') {
    if(escaped) {
      escaped = 0;
      p++;
      continue;
    }

    if(*p == '\\') {
      escaped = 1;
      p++;
      continue;
    }

    if(*p == '\'') {
      return (char*)p;
    }

    p++;
  }

  return NULL;
}

char *fstring_replace(char *str, int n)
{
  // prevent infinite recursion
  if(n > 16) {
    return str;
  }

  if(strstr(str, "f'") == NULL) {
    return str;
  }

  char *ptr = str;

  int was_f = 0;
  for(int i = 0; i < strlen(str); i++) {
    if(str[i] == 'f') {
      was_f = 1;
      continue;
    }

    if(was_f && (str[i] == '\'')) {
      was_f = 0;
      ptr = str + i + 1;
      break;
    }

    if(was_f) {
      was_f = 0;
    }
  }

  // Find the closing quote to extract only the f-string content
  char *ptr_quote_end = find_closing_quote(ptr);
  if(ptr_quote_end == NULL) {
    return str;  // Malformed, no closing quote
  }

  // Extract content between f' and closing '
  int content_len = ptr_quote_end - ptr;
  char *content = (char*)malloc(content_len + 1);
  if(content == NULL) {
    return str;
  }
  strncpy(content, ptr, content_len);
  content[content_len] = '\0';

  Fstring *f = NULL;

  // Extract variables from {} braces in the content
  char buffer[256];
  int buffer_len = 0;
  int curly_braces = 0;
  for(int i = 0; i < content_len; i++) {
    if(content[i] == '{') {
      curly_braces++;
      continue;
    }

    if(curly_braces > 0 && content[i] != '}'){
      buffer[buffer_len] = content[i];
      buffer_len++;
    }

    if(content[i] == '}') {
      curly_braces--;

      if(curly_braces == 0) {
        buffer[buffer_len] = '\0';
        push_fstring(&f, buffer);
        buffer_len = 0;
        continue;
      }
    }
  }

  if(f != NULL) {
    char *replacement = (char*)malloc(strlen(f->value) + 3);
    sprintf(replacement, "{%s}", f->value);
    char *newstr = str_replace(content, replacement, "%s");

    Fstring *curr = f->next;
    while(curr != NULL) {
      replacement = (char*)realloc(replacement, strlen(curr->value) + 3);
      sprintf(replacement, "{%s}", curr->value);
      char *old_newstr = newstr;
      newstr = str_replace(newstr, replacement, "%s");
      free(old_newstr);
      curr = curr->next;
    }

    free(replacement);

    // Build argument list: ", var1, var2, var3"
    char args_list[1024] = "";
    Fstring *arg_curr = f;
    while(arg_curr != NULL) {
      strcat(args_list, ", ");
      strcat(args_list, arg_curr->value);
      arg_curr = arg_curr->next;
    }

    // Find where the f-string starts and ends
    char *fstring_start = strstr(str, "f'");
    char *content_start = fstring_start + 2;
    char *quote_end = find_closing_quote(content_start);

    if(fstring_start == NULL || quote_end == NULL) {
      free(content);
      free_fstring(f);
      return str;  // Malformed, return unchanged
    }

    // Build sprintf call: sprintf('format_string', arg1, arg2, ...)
    int sprintf_len = strlen("sprintf('") + strlen(newstr) + strlen("')") + strlen(args_list) + 1;
    char *sprintf_call = (char*)malloc(sprintf_len);
    if(sprintf_call == NULL) {
      free(content);
      free_fstring(f);
      return str;
    }
    sprintf(sprintf_call, "sprintf('%s'%s)", newstr, args_list);
    free(newstr);

    // Replace the entire f'...' with sprintf(...)
    int prefix_len = fstring_start - str;
    int suffix_start = (quote_end - str) + 1;
    char *result = (char*)malloc(prefix_len + strlen(sprintf_call) + strlen(str + suffix_start) + 1);
    if(result == NULL) {
      free(sprintf_call);
      free(content);
      free_fstring(f);
      return str;
    }
    strncpy(result, str, prefix_len);
    result[prefix_len] = '\0';
    strcat(result, sprintf_call);
    strcat(result, str + suffix_start);

    free(sprintf_call);
    free(content);
    free_fstring(f);

    char *final_result = fstring_replace(result, n + 1);
    if(final_result != result) {
      free(result);
    }
    return final_result;
  } else {
    char *fstring_start = strstr(str, "f'");
    if(fstring_start == NULL) {
      free(content);
      return str;
    }

    char *content_start = fstring_start + 2;
    char *quote_end = find_closing_quote(content_start);

    if(quote_end == NULL) {
      free(content);
      return str;
    }

    int sprintf_len = strlen("sprintf('") + strlen(content) + strlen("')") + 1;
    char *sprintf_call = (char*)malloc(sprintf_len);
    if(sprintf_call == NULL) {
      free(content);
      return str;
    }
    sprintf(sprintf_call, "sprintf('%s')", content);

    int prefix_len = fstring_start - str;
    int suffix_start = (quote_end - str) + 1;
    char *result = (char*)malloc(prefix_len + strlen(sprintf_call) + strlen(str + suffix_start) + 1);
    if(result == NULL) {
      free(sprintf_call);
      free(content);
      return str;
    }
    strncpy(result, str, prefix_len);
    result[prefix_len] = '\0';
    strcat(result, sprintf_call);
    strcat(result, str + suffix_start);

    free(sprintf_call);
    free(content);

    // Recursively process any remaining f-strings
    char *final_result = fstring_replace(result, n + 1);
    if(final_result != result) {
      free(result);
    }
    return final_result;
  }
}
