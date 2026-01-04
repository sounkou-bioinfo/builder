#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Rinternals.h>
#include <R_ext/Parse.h>
#include "log.h"

void set_R_home()
{
  if(getenv("R_HOME") != NULL) return;

  FILE *fp = popen("R RHOME", "r");
  if(fp == NULL) {
    printf("%s Failed to run R RHOME and R_HOME environment variables are not set\n", LOG_ERROR);
    return;
  }

  char path[512];
  if(fgets(path, sizeof(path), fp) != NULL) {
    path[strcspn(path, "\n")] = 0;
    setenv("R_HOME", path, 1);
    printf("%s Setting R_HOME environment variable to `%s` (R RHOME)\n", LOG_WARNING, path);
  } else {
    printf("%s Failed to run R RHOME and R_HOME environment variables are not set\n", LOG_ERROR);
  }

  pclose(fp);
}

char *remove_trailing_newline(char *line)
{
  size_t len = strlen(line);
  if(len > 0 && line[len - 1] == '\n') {
    line[len - 1] = '\0';
  }
  return line;
}

int eval_if(char *expr)
{
  SEXP code_sexp, result;
  ParseStatus status;
  int has_error;

  PROTECT(code_sexp = mkString(expr));

  SEXP parsed = PROTECT(R_ParseVector(code_sexp, -1, &status, R_NilValue));

  if (status != PARSE_OK) {
    UNPROTECT(2);
    printf("%s Parsing expression `%s`\n", LOG_ERROR, remove_trailing_newline(expr));
    return 0;
  }

  result = PROTECT(R_tryEval(VECTOR_ELT(parsed, 0), R_GlobalEnv, &has_error));

  if (has_error) {
    UNPROTECT(3);
    printf("%s Evaluating expression `%s`\n", LOG_ERROR, remove_trailing_newline(expr));
    return 0;
  }

  UNPROTECT(3);

  if(TYPEOF(result) != LGLSXP) {
    printf("%s Expression `%s` did not evaluate to a logical value\n", LOG_ERROR, remove_trailing_newline(expr));
    return 0;
  }

  return asLogical(result);
}
