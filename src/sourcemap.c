#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *add_sourcemap(char *line, int line_number)
{
  // there's a special comment in the R code that we don't want to touch
  if(strstr(line, "#") != NULL) {
    return line;
  }

  char *p = line;
  while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
  if(*p == '\0') {
    return line;
  }

  char *line_str = NULL;
  asprintf(&line_str, "# line: %d", line_number);

  size_t len = strlen(line);
  if(len > 0 && line[len - 1] == '\n') {
    line[len - 1] = '\0';
  }

  char *new_line = malloc(strlen(line) + strlen(line_str) + 3);
  snprintf(new_line, strlen(line) + strlen(line_str) + 3, "%s %s\n", line, line_str);
  free(line);
  free(line_str);
  return new_line;
}
