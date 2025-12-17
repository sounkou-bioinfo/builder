#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <regex.h>
#include "file.h"
#include "log.h"

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
  char *dir = strdup(path);
  char last_slash = dir[strlen(dir) - 1];
  if(last_slash == '/') {
    dir = realloc(dir, strlen(dir) - 1);
  }
  return dir;
}

char *ensure_dir(char *path)
{
  char *dir = strdup(path);
  char last_slash = dir[strlen(dir) - 1];
  if(last_slash != '/') {
    dir = realloc(dir, strlen(dir) + 2);
    strcat(dir, "/");
  }
  return dir;
}

int clean(char *src, char *dst, int n_extra_args, char **extra_args)
{
  return remove(src);
}

int matches_def(char line[], int n_extra_args, char **extra_args)
{
  for(int i = 0; i < n_extra_args; i++) {
    regex_t regex;
    regcomp(&regex, extra_args[i], REG_EXTENDED);
    if(regexec(&regex, line, 0, NULL, 0) == 0) {
      regfree(&regex);
      return 1;
    }

    regfree(&regex);
  }

  return 0;
}

char *remove_leading_spaces(char *line)
{
  while(*line && (*line == ' ' || *line == '\t')) {
    line++;
  }
  return line;
}

int get_state(int state, char line[1024], int n_extra_args, char **extra_args)
{
  // empty line
  if(strlen(line) == 0) {
    return state;
  }

  // line is too short to include a directive
  if(strlen(line) < 6) {
    return state;
  }

  char *trimmed = remove_leading_spaces(line);

  // line is nto a comment
  if(trimmed[0] != '#') {
    return state;
  }

  if(strncmp(trimmed, "#ifdef", 6) == 0) {
    return matches_def(trimmed, n_extra_args, extra_args);
  }

  if(strncmp(trimmed, "#endif", 6) == 0) {
    return 1;
  }

  if(strlen(trimmed) < 7) {
    return state;
  }

  if(strncmp(trimmed, "# ifdef", 7) == 0){
    return matches_def(trimmed, n_extra_args, extra_args);
  }

  if(strncmp(trimmed, "# endif",  7) == 0) {
    return 1;
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
    log_error("Failed to allocate memory");
    return NULL;
  }

  snprintf(path, l, "%s%s", dst, replace_slash(src));
  return path;
}

int copy(char *src, char *dst, int n_extra_args, char **extra_args)
{
  char *dest = make_dest_path(src, dst);
  FILE *src_file = fopen(src, "r");
  FILE *dst_file = fopen(dest, "w");

  if(src_file == NULL) {
    fclose(dst_file);
    fclose(src_file);
    log_error("Failed to open source file");
    return 1;
  }

  char *msg = malloc(strlen(src) + strlen(dst) + 64);
  snprintf(msg, strlen(src) + strlen(dst) + 64, "Copying %s to %s", src, dest);
  log_info(msg);
  free(msg);

  char line[1024];
  int should_write = 1;
  int previous_state = 1;
  while(fgets(line, 1024, src_file) != NULL) {
    previous_state = should_write;
    should_write = get_state(should_write, line, n_extra_args, extra_args);

    if(!should_write) {
      continue;
    }

    if(should_write && !previous_state) {
      continue;
    }

    if(fputs(line, dst_file) == EOF) {
      log_error("Failed to write to destination file");
      return 1;
    }
  }

  fclose(dst_file);
  fclose(src_file);

  return 0;
}

int transfer(char *src_dir, char *dst_dir, int n_extra_args, char **extra_args, Callback func)
{
  DIR *source;
  struct dirent *entry;
  char path[PATH_MAX];
  
  source = opendir(src_dir);
  if (source == NULL) {
    char msg[PATH_MAX + 64];
    snprintf(msg, sizeof(msg), "Failed to open source directory: %s", src_dir);
    log_error(msg);
    return 1;
  }

  while((entry = readdir(source)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 ||
        strcmp(entry->d_name, "..") == 0) {
        continue;
    }

    snprintf(path, PATH_MAX, "%s/%s", src_dir, entry->d_name);

    // it's a directory, recurse
    if (entry->d_type == DT_DIR) {
      transfer(path, dst_dir, n_extra_args, extra_args, func);
    } else {
      func(path, dst_dir, n_extra_args, extra_args);
    }
  }
  
  closedir(source);
  return 0;
}
