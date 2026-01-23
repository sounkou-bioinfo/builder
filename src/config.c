#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "parser.h"
#include "log.h"

#define MAX_LINE 1024

int has_config()
{
  FILE *fp = fopen("builder.ini", "r");
  if (fp == NULL) return 0;
  fclose(fp);
  return 1;
}

static char *get_value(char *line)
{
  char *colon = strchr(line, ':');
  if (colon == NULL) return NULL;

  char *value = colon + 1;
  while (*value == ' ') value++;

  size_t len = strlen(value);
  if (len > 0 && value[len - 1] == '\n') {
    value[len - 1] = '\0';
  }

  return strdup(value);
}

static int get_bool(char *line)
{
  char *value = get_value(line);
  if (value == NULL) return 0;

  int result = strcmp(value, "true") == 0;
  free(value);
  return result;
}

static Value *parse_values(char *line)
{
  char *str = get_value(line);
  if (str == NULL) return NULL;

  Value *values = NULL;
  char *token = strtok(str, " ");

  while (token != NULL) {
    values = push_value(values, strdup(token));
    token = strtok(NULL, " ");
  }

  free(str);
  return values;
}

BuildContext *get_config()
{
  FILE *fp = fopen("builder.ini", "r");
  if (fp == NULL) return NULL;
  printf("%s Using config file: builder.ini\n", LOG_INFO);

  BuildContext *ctx = malloc(sizeof(BuildContext));
  if (ctx == NULL) {
    fclose(fp);
    return NULL;
  }

  ctx->argc = 0;
  ctx->argv = NULL;
  ctx->input = NULL;
  ctx->output = NULL;
  ctx->imports = NULL;
  ctx->prepend = NULL;
  ctx->append = NULL;
  ctx->plugins_str = NULL;
  ctx->plugins = NULL;
  ctx->deadcode = 0;
  ctx->sourcemap = 0;
  ctx->must_clean = 1;
  ctx->watch = 0;

  char line[MAX_LINE];
  while (fgets(line, MAX_LINE, fp) != NULL) {
    if (line[0] == '#') continue;
    if (line[0] == '\n') continue;

    if (strstr(line, "input:") != NULL) {
      ctx->input = get_value(line);
      continue;
    }

    if (strstr(line, "output:") != NULL) {
      ctx->output = get_value(line);
      continue;
    }

    if (strstr(line, "prepend:") != NULL) {
      ctx->prepend = get_value(line);
      continue;
    }

    if (strstr(line, "append:") != NULL) {
      ctx->append = get_value(line);
      continue;
    }

    if (strstr(line, "deadcode:") != NULL) {
      ctx->deadcode = get_bool(line);
      continue;
    }

    if (strstr(line, "sourcemap:") != NULL) {
      ctx->sourcemap = get_bool(line);
      continue;
    }

    if (strstr(line, "clean:") != NULL) {
      ctx->must_clean = get_bool(line);
      continue;
    }

    if (strstr(line, "watch:") != NULL) {
      ctx->watch = get_bool(line);
      continue;
    }

    if (strstr(line, "plugin:") != NULL) {
      Value *plugins = parse_values(line);
      if (ctx->plugins_str == NULL) {
        ctx->plugins_str = plugins;
      } else {
        Value *current = ctx->plugins_str;
        while (current->next != NULL) current = current->next;
        current->next = plugins;
      }
      continue;
    }

    if (strstr(line, "import:") != NULL) {
      Value *imports = parse_values(line);
      if (ctx->imports == NULL) {
        ctx->imports = imports;
      } else {
        Value *current = ctx->imports;
        while (current->next != NULL) current = current->next;
        current->next = imports;
      }
      continue;
    }
  }

  fclose(fp);
  return ctx;
}

void free_config(BuildContext *ctx)
{
  if (ctx == NULL) return;
  free(ctx->input);
  free(ctx->output);
  free(ctx->prepend);
  free(ctx->append);
  free_value(ctx->imports);
  free_value(ctx->plugins_str);
  free(ctx);
}
