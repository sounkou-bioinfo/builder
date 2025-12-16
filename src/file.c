#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <regex.h>
#include "file.h"
#include "log.h"

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
  return 0;
}

int is_def(int state, char line[1024])
{
  if(strlen(line) == 0) {
    return state;
  }

  if(line[0] != '#') {
    return state;
  }

  if(strlen(line) < 6) {
    return state;
  }

  if(strncmp(line, "#ifdef", 6) == 0) {
    return 1;
  }

  if(strncmp(line, "#endif", 6) == 0) {
    return 0;
  }

  if(strlen(line) < 7) {
    return state;
  }

  if(strncmp(line, "# ifdef", 7) == 0){
    return 1;
  }

  if(strncmp(line, "# endif",  7) == 0) {
    return 0;
  }

  return state;
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

int copy(char *src, char *dst, int n_extra_args, char **extra_args)
{

  FILE *src_file = fopen(src, "r");

  if(src_file == NULL) {
    log_error("Failed to open source file");
    return 1;
  }

  printf("Copying %s to %s\n", src, dst);

  char line[1024];
  int is_in_def = 0;
  while(fgets(line, 1024, src_file) != NULL) {
    is_in_def = is_def(is_in_def, line);
    printf("#%d", is_in_def);
    printf("%s", line);
  }

  if (is_in_def > 0) {
    log_error("Failed to find matching #endif");
    return 1;
  }
    
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
