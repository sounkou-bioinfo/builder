#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <regex.h>

#include "deconstruct.h"
#include "define.h"
#include "include.h"
#include "fstring.h"
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

int clean(char *src, char *dst, Define **defs)
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

int copy(char *src, char *dst, Define **defs)
{
  char *dest = make_dest_path(src, dst);
  FILE *src_file = fopen(src, "r");
  FILE *dst_file = fopen(dest, "w");

  if(src_file == NULL) {
    fclose(dst_file);
    fclose(src_file);
    free(dest);
    printf("%s Failed to open source file\n", LOG_ERROR);
    return 1;
  }

  overwrite(defs, "__FILE__", src);

  printf("%s Copying %s to %s\n", LOG_INFO, src, dest);

  size_t line_len = 1024;
  char line[line_len];
  int should_write = 1;
  int i = 0;
  char *istr = NULL;


  Tests *tests = NULL;

  int is_macro = 0;
  int max_macro_lines = 1024;
  while(fgets(line, line_len, src_file) != NULL) {
    i++;
    asprintf(&istr, "%d", i);
    overwrite(defs, "__LINE__", istr);
    free(istr);

    // this is such a fukcing mess man
    is_macro = define(defs, line);
    if(is_macro) {
      char *macro;
      define_macro_init(&macro, line);

      // initial number of braces on first line
      // #define macro_name(args) {...}
      int braces = count_braces(line);
      char *macro_name = define_macro(line);

      int macro_lines = 0;
      while(braces > 0 && macro_lines < max_macro_lines) {
        while(fgets(line, line_len, src_file) != NULL) {
          macro_lines++;
          braces += count_braces(line);

          if(strncmp(line, "# ", 2) == 0) {
            memmove(line, line + 2, strlen(line + 2) + 1);
          }

          if(strncmp(line, "#", 1) == 0) {
            memmove(line, line + 1, strlen(line + 1) + 1);
          }

          size_t l = strlen(macro) + strlen(line) + 1;
          macro = realloc(macro, l);
          strcat(macro, line);

          // we're closing the macro
          if(braces <= 0) {
            break;
          }
        }
      }

      is_macro = 0;
      if(macro_lines == max_macro_lines) {
        printf("%s macro %s is too long (or failed to parse), reached %d lines\n", LOG_WARNING, macro, macro_lines);
        free(macro);
        free(macro_name);
      }

      push((*defs), macro_name, macro, DEF_FUNCTION);

      continue;
    }

    char *fstring_result = fstring_replace(line, 0);
    char *replaced = define_replace(defs, fstring_result);
    char *processed = include_replace(defs, replaced);
    if(processed != replaced) {
      free(replaced);
    }
    char *deconstructed = deconstruct_replace(processed);
    char *processed_copy = strdup(deconstructed);

    int test = enter_test(processed);
    if(test) {
      char *t = remove_leading_spaces(processed);
      char *description = strdup(t + 6);
      // Remove trailing newline from description
      size_t desc_len = strlen(description);
      if(desc_len > 0 && description[desc_len - 1] == '\n') {
        description[desc_len - 1] = '\0';
      }
      char *expressions = NULL;
      while(fgets(line, line_len, src_file) != NULL) {
        t = remove_leading_spaces(line);
        if(strncmp(t, "#endtest", 8) == 0) {
          Tests *new_test = create_test(description, expressions);
          push_test(&tests, new_test);
          break;
        }

        if(expressions == NULL) {
          expressions = (char *)malloc(strlen(t) - 1 + 1);
          strcpy(expressions, t + 1);
          continue;
        }

        expressions = realloc(expressions, strlen(expressions) + strlen(t) - 1 + 1);
        strcat(expressions, t + 1);
      }
    }

    should_write = should_write_line(should_write, processed_copy, defs);
    free(processed_copy);

    if(!should_write) {
      free(deconstructed);
      if(processed != deconstructed) {
        free(processed);
      }
      if(fstring_result != line) {
        free(fstring_result);
      }
      continue;
    }

    if(fputs(deconstructed, dst_file) == EOF) {
      free(deconstructed);
      if(processed != deconstructed) {
        free(processed);
      }
      if(fstring_result != line) {
        free(fstring_result);
      }
      free(dest);
      printf("%s Failed to write to destination file\n", LOG_ERROR);
      return 1;
    }

    free(deconstructed);
    if(processed != deconstructed) {
      free(processed);
    }
    if(fstring_result != line) {
      free(fstring_result);
    }
  }

  write_tests(tests, src);

  free(dest);
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
