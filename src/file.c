#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <regex.h>

#include "deconstruct.h"
#include "preflight.h"
#include "plugins.h"
#include "import.h"
#include "define.h"
#include "include.h"
#include "fstring.h"
#include "const.h"
#include "file.h"
#include "test.h"
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
    char *dir = malloc(len);
    if(dir == NULL) {
      return NULL;
    }
    strncpy(dir, path, len - 1);
    dir[len - 1] = '\0';
    free(path);
    return dir;
  }

  return path;
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
    free(path);  // Free the input since we're returning a new allocation
    return dir;
  }

  // Already ends with '/', return the same pointer
  return path;
}

int clean(char *src, char *dst, Define **defs, Plugins *plugins)
{
  // Remove the output file
  int result = remove(src);

  // Also remove the corresponding test file
  // Extract filename from src (e.g., "R/sub-main.R" -> "sub-main.R")
  const char *filename = strrchr(src, '/');
  if(filename == NULL) {
    filename = src;
  } else {
    filename++;  // Skip the '/'
  }

  // Construct test filename: tests/testthat/test-builder-<filename>
  size_t test_path_len = strlen("tests/testthat/test-builder-") + strlen(filename) + 1;
  char *test_path = malloc(test_path_len);
  if(test_path != NULL) {
    snprintf(test_path, test_path_len, "tests/testthat/test-builder-%s", filename);

    // Remove test file if it exists (don't report error if it doesn't)
    remove(test_path);

    free(test_path);
  }

  return result;
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

  if(strncmp(trimmed, "#test", 10) == 0) {
    return 0;
  }

  if(strncmp(trimmed, "#endtest", 10) == 0) {
    return 1;
  }

  if(strncmp(trimmed, "#preflight", 10) == 0) {
    return 0;
  }

  if(strncmp(trimmed, "#endflight", 10) == 0) {
    return 1;
  }

  if(strncmp(trimmed, "#endpreflight", 13) == 0) {
    return 1;
  }

  if(strncmp(trimmed, "#ifdef", 6) == 0) {
    char *keyword = remove_keyword(trimmed);
    int result = get_define_value(defs, keyword) != NULL;
    free(keyword);
    return result;
  }

  if(strncmp(trimmed, "#ifndef", 7) == 0) {
    char *keyword = remove_keyword(trimmed);
    int result = get_define_value(defs, keyword) == NULL;
    free(keyword);
    return result;
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

  char *replaced = replace_slash(src);
  snprintf(path, l, "%s%s", dst, replaced);
  free(replaced);

  char *slash = strchr(path, '/');
  char *start = (slash != NULL) ? slash + 1 : path;
  char *hyphen = strchr(start, '-');

  if(hyphen == NULL) {
    return path;
  }

  memmove(start, hyphen + 1, strlen(hyphen + 1) + 1);

  return path;
}

int count_braces(char *line)
{
  int count = 0;
  for(int i = 0; i < strlen(line); i++) {
    if(line[i] == '{') {
      count++;
    }

    if(line[i] == '}') {
      count--;
    }
  }

  return count;
}

char *append_buffer(char *buffer, char *line)
{
  if(buffer == NULL) {
    return strdup(line);
  }

  char *new_buffer = malloc(strlen(buffer) + strlen(line) + 2);
  if(new_buffer == NULL) {
    return NULL;
  }

  strcpy(new_buffer, buffer);
  strcat(new_buffer, "\n");
  strcat(new_buffer, line);
  free(buffer);

  return new_buffer;
}

int walk(char *src_dir, char *dst_dir, Callback func, Define **defs, Plugins *plugins)
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
      walk(path, dst_dir, func, defs, plugins);
    } else {
      char *ext = strrchr(path, '.');
      if(strcmp(ext, ".R") != 0 && strcmp(ext, ".r") != 0) continue;
      func(path, dst_dir, defs, plugins);
    }
  }
  
  closedir(source);
  return 0;
}

RFile *create_rfile(char *src, char *dst, char *content)
{
  RFile *file = malloc(sizeof(RFile));
  if(file == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return NULL;
  }

  file->src = strdup(src);
  file->dst = strdup(dst);

  // content is optional
  if(content != NULL) {
    file->content = strdup(content);
  } else {
    file->content = NULL;
  }
  file->next = NULL;

  return file;
}

void push_rfile(RFile **files, char *src, char *dst, char *content)
{
  RFile *file = create_rfile(src, dst, content);
  if(file == NULL) {
    return;
  }

  file->next = *files;
  *files = file;
}

int collect_files(RFile **files, char *src_dir, char *dst_dir)
{
  DIR *source;
  struct dirent *entry;
  char path[PATH_MAX];
  
  source = opendir(src_dir);
  if (source == NULL) {
    printf("%s Failed to open source directory: %s\n", LOG_ERROR, src_dir);
    return 0;
  }

  while((entry = readdir(source)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    snprintf(path, PATH_MAX, "%s/%s", src_dir, entry->d_name);

    // it's a directory, recurse
    if (entry->d_type == DT_DIR) {
      collect_files(files, path, dst_dir);
    } else {
      char *ext = strrchr(path, '.');
      if(strcmp(ext, ".R") != 0 && strcmp(ext, ".r") != 0) continue;
      char buffer[20000];
      FILE *file = fopen(path, "r");
      size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
      buffer[bytesRead] = '\0';
      char *dest = make_dest_path(path, dst_dir);
      push_rfile(files, path, dest, buffer);
      fclose(file);
    }
  }
  
  closedir(source);

  return 1;
}

// first pass:
// - capture defines
// - Run preflight
int first_pass(RFile *files, Define **defs, Plugins *plugins)
{
  RFile *current = files;
  while(current != NULL) {
    overwrite(defs, "__FILE__", current->src);

    // state
    char *buffer = NULL;
    int line_number = -1;
    char *line_number_str = NULL;
    int in_preflight = 0;
    int in_macro = 0;

    // line
    char *pos = current->content;

    while (*pos) {
      line_number++;
      char *new_line = strchr(pos, '\n');
      if(!new_line) {
        break;
      }

      size_t len = new_line - pos;
      char *line = malloc(len + 1);
      strncpy(line, pos, len);
      line[len] = '\0';
      pos = new_line + 1;

      asprintf(&line_number_str, "%d", line_number);
      overwrite(defs, "__LINE__", line_number_str);

      if(strncmp(line, "#enddef", 7) == 0) {
        in_macro = 0;
        push_macro(defs, buffer, NULL);
        buffer = NULL;
        continue;
      }

      if(in_macro) {
        buffer = append_buffer(buffer, line);
        continue;
      }

      if(define(defs, line, NULL)) {
        in_macro = 1;
        continue;
      }

      if(strncmp(line, "#preflight", 10) == 0) {
        in_preflight = 1;
        buffer = append_buffer(buffer, line);
        continue;
      }

      if(strncmp(line, "#endpreflight", 13) == 0) {
        in_preflight = 0;
        continue;
      }

      if(strncmp(line, "#endflight", 10) == 0) {
        in_preflight = 0;
        printf("%s Running preflight checks\n", LOG_INFO);
        SEXP result = evaluate(buffer);
        if(result == NULL) {
          printf("%s Preflight checks failed\n", LOG_ERROR);
          return 1;
        }
        buffer = NULL;
        continue;
      }

      if(in_preflight) {
        buffer = append_buffer(buffer, line);
        continue;
      }
    }

    char *output = plugins_call(plugins, "preprocess", current->content, current->src);
    if(output != NULL) {
      free(current->content);
      current->content = strdup(output);
      free(output);
    }

    current = current->next;
  }

  return 0;
}

int second_pass(RFile *files, Define **defs, Plugins *plugins)
{
  RFile *current = files;
  while(current != NULL) {
    printf("%s Copying %s to %s\n", LOG_INFO, current->src, current->dst);
    overwrite(defs, "__FILE__", current->src);

    // state
    char *buffer = NULL;
    int line_number = -1;
    char *line_number_str = NULL;
    int should_write = 1;

    // Test collector
    TestCollector tc = {NULL, 0, NULL, NULL};

    // line
    char *pos = current->content;

    while (*pos) {
      line_number++;
      char *new_line = strchr(pos, '\n');
      if(!new_line) {
        break;
      }

      size_t len = new_line - pos;
      char *line = malloc(len + 1);
      strncpy(line, pos, len);
      line[len] = '\0';
      pos = new_line + 1;

      asprintf(&line_number_str, "%d", line_number);
      overwrite(defs, "__LINE__", line_number_str);

      char *fstring_result = fstring_replace(line, 0);
      char *replaced = define_replace(defs, fstring_result);
      char *processed = include_replace(defs, replaced, plugins, current->src);
      char *deconstructed = deconstruct_replace(processed);
      char *cnst = replace_const(deconstructed);

      // Test collection - if line was consumed, skip to next
      if(collect_test_line(&tc, cnst)) {
        free(line);
        continue;
      }

      // modify content
      should_write = should_write_line(should_write, cnst, defs);

      if(!should_write) {
        free(line);
        continue;
      }
      buffer = append_buffer(buffer, cnst);
      free(line);
    }

    FILE *dst_file = fopen(current->dst, "w");
    if(dst_file == NULL) {
      printf("%s Failed to open %s\n", LOG_ERROR, current->dst);
      return 1;
    }
    char *output = plugins_call(plugins, "postprocess", buffer, current->src);
    if(output != NULL) {
      fputs(output, dst_file);
      free(output);
    } else if(buffer != NULL) {
      fputs(buffer, dst_file);
    }
    fclose(dst_file);
    free(buffer);

    write_tests(tc.tests, current->src);

    current = current->next;
  }

  return 0;
}

int two_pass(RFile *files, Define **defs, Plugins *plugins)
{
  int first_pass_result = first_pass(files, defs, plugins);
  if(first_pass_result) {
    return 1;
  }

  return second_pass(files, defs, plugins);
}
