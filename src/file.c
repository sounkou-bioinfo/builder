#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
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

int copy(char *src, char *dst, int n_extra_args, char **extra_args)
{

  FILE *src_file = fopen(src, "r");

  if(src_file == NULL) {
    log_error("Failed to open source file");
    return 1;
  }

  printf("Copying %s to %s\n", src, dst);

  char line[1024];
  while(fgets(line, 1024, src_file) != NULL) {
    printf("%s", line);
  }

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
