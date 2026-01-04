#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <regex.h>
#include "define.h"
#include "file.h"
#include "log.h"
#include "r.h"

int exists(char *path)
{
  FILE *file = fopen(path, "r");

  if(file == NULL) {
    return 0;
  }

  fclose(file);
  return 1;
}

char *strip_last_slash(char *path)
{
  size_t len = strlen(path);

  if(len > 0 && path[len - 1] == '/') {
    char *dir = malloc(len);  // len - 1 for string + 1 for null = len
    if(dir == NULL) {
      return NULL;
    }
    strncpy(dir, path, len - 1);
    dir[len - 1] = '\0';
    return dir;
  }

  return strdup(path);
}

char *ensure_dir(char *path)
{
  size_t len = strlen(path);

  if(len > 0 && path[len - 1] != '/') {
    char *dir = malloc(len + 2);  // len + '/' + null terminator
    if(dir == NULL) {
      return NULL;
    }
    strcpy(dir, path);
    strcat(dir, "/");
    return dir;
  }

  return strdup(path);
}

int clean(char *src, char *dst, Define **defs)
{
  return remove(src);
}

char *remove_leading_spaces(char *line)
{
  while(*line && (*line == ' ' || *line == '\t')) {
    line++;
  }
  return line;
}

char *remove_keyword(char *line)
{
  char *t = strchr(line, ' ');
  if(t == NULL) {
    return strdup(line);
  }
  t++;

  size_t len = strlen(t);
  if(len > 0 && t[len - 1] == '\n') {
    t[len - 1] = '\0';
  }

  return strdup(t);
}

int should_write_line(int state, char line[1024], Define **defs)
{
  char *trimmed = remove_leading_spaces(line);

  // line is not a comment
  if(trimmed[0] != '#') {
    return state;
  }

  if(strncmp(trimmed, "#ifdef", 6) == 0) {
    return get_define_value(defs, remove_keyword(trimmed)) != NULL;
  }

  if(strncmp(trimmed, "#ifndef", 7) == 0) {
    return get_define_value(defs, remove_keyword(trimmed)) == NULL;
  }

  if(strncmp(trimmed, "#else", 5) == 0) {
    if(state == 1) return 0;
    return 1;
  }

  if(strncmp(trimmed, "#endif", 6) == 0) {
    return 1;
  }

  if(strncmp(trimmed, "#if", 3) == 0) {
    // remove #if
    memmove(trimmed, trimmed + 4, strlen(trimmed + 4) + 1);
    return eval_if(trimmed);
  }

  return state;
}

char *replace_slash(char *path)
{
  char *new_path = strdup(path);
  for(int i = 0; i < strlen(new_path); i++) {
    if(new_path[i] == '/') {
      new_path[i] = '-';
    }
  }
  
  return new_path;
}

char *make_dest_path(char *src, char *dst)
{
  size_t l = strlen(src) + strlen(dst) + 1;
  char *path = malloc(l);

  if(path == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return NULL;
  }

  snprintf(path, l, "%s%s", dst, replace_slash(src));

  char *slash = strchr(path, '/');
  char *start = (slash != NULL) ? slash + 1 : path;
  char *hyphen = strchr(start, '-');

  if(hyphen == NULL) {
    return path;
  }

  memmove(start, hyphen + 1, strlen(hyphen + 1) + 1);

  return path;
}

int copy(char *src, char *dst, Define **defs)
{
  char *dest = make_dest_path(src, dst);
  FILE *src_file = fopen(src, "r");
  FILE *dst_file = fopen(dest, "w");

  if(src_file == NULL) {
    fclose(dst_file);
    fclose(src_file);
    printf("%s Failed to open source file\n", LOG_ERROR);
    return 1;
  }

  overwrite(defs, "__FILE__", strdup(src));

  printf("%s Copying %s to %s\n", LOG_INFO, src, dest);

  char line[1024];
  int should_write = 1;
  int i = 0;
  char *istr;
  while(fgets(line, 1024, src_file) != NULL) {
    i++;
    asprintf(&istr, "%d", i);
    overwrite(defs, "__LINE__", istr);
    define(defs, line);
    char *processed = define_replace(defs, line);
    should_write = should_write_line(should_write, strdup(processed), defs);

    if(!should_write) {
      free(processed);
      continue;
    }

    if(fputs(processed, dst_file) == EOF) {
      free(processed);
      printf("%s Failed to write to destination file\n", LOG_ERROR);
      return 1;
    }

    free(processed);
  }

  free(istr);
  fclose(dst_file);
  fclose(src_file);

  return 0;
}

int walk(char *src_dir, char *dst_dir, Callback func, Define **defs)
{
  DIR *source;
  struct dirent *entry;
  char path[PATH_MAX];
  
  source = opendir(src_dir);
  if (source == NULL) {
    printf("%s Failed to open source directory: %s\n", LOG_ERROR, src_dir);
    return 1;
  }

  while((entry = readdir(source)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    snprintf(path, PATH_MAX, "%s/%s", src_dir, entry->d_name);

    // it's a directory, recurse
    if (entry->d_type == DT_DIR) {
      walk(path, dst_dir, func, defs);
    } else {
      func(path, dst_dir, defs);
    }
  }
  
  closedir(source);
  return 0;
}
