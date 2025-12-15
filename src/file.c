#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include "file.h"
#include "log.h"

int clean(char *src, char *dst, int n_extra_args, char **extra_args)
{
  return 0;
}

int copy(char *src, char *dst, int n_extra_args, char **extra_args)
{
  return 0;
}

int transfer(char *src_dir, char *dst_dir, int n_extra_args, char **extra_args, Callback func)
{
  DIR *source;
  struct dirent *entry;
  char path[PATH_MAX];  // New buffer for the full path
  
  source = opendir(src_dir);
  if (source == NULL) {
  char *msg = strdup("Failed to open source directory: ");
  strcat(msg, src_dir);
  log_error(msg);
  free(msg);
  return 1;
}

while((entry = readdir(source)) != NULL) {
  if (strcmp(entry->d_name, ".") == 0 || 
      strcmp(entry->d_name, "..") == 0) {
      continue;
  }
  
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
