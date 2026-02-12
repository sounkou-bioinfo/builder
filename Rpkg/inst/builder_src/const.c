#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *replace_const(char *line)
{
  char *pos = strstr(line, "-<");
  if (pos == NULL) return line;

  // LHS
  size_t lhs_len = pos - line;
  char *lhs = malloc(lhs_len + 1);
  strncpy(lhs, line, lhs_len);
  lhs[lhs_len] = '\0';

  while(lhs_len > 0 && isspace(lhs[lhs_len - 1])) {
    lhs_len--;
    lhs[lhs_len] = '\0';
  }

  // RHS
  pos += 2;
  while(isspace(*pos)) pos++;
  if(pos[strlen(pos)-1] == '\n') pos[strlen(pos)-1] = '\0';

  for(int i = 0; i < strlen(line); i++) {
    if(line[i] == '-') {
      line[i] = '<';
      continue;
    }

    if(line[i] == '<') {
      line[i] = '-';
      break;
    }
  }

  char *nl = (char *)malloc(strlen(";lockBinding(\"") + strlen(lhs) + strlen("\", environment());") + 1);
  strcpy(nl, line);
  strcat(nl, ";lockBinding(\"");
  strcat(nl, lhs);
  strcat(nl, "\", environment());");
  free(lhs);

  return nl;
}
