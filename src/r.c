#include <stdio.h>
#include <string.h>
#include <Rinternals.h>
#include <R_ext/Parse.h>

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
    printf("[ERROR] Parsing expression `%s`\n", remove_trailing_newline(expr));
    return 0;
  }

  result = PROTECT(R_tryEval(VECTOR_ELT(parsed, 0), R_GlobalEnv, &has_error));

  if (has_error) {
    UNPROTECT(3);
    printf("[ERROR] Evaluating expression `%s`\n", remove_trailing_newline(expr));
    return 0;
  }

  UNPROTECT(3);

  if(TYPEOF(result) != LGLSXP) {
    printf("[ERROR] Expression `%s` did not evaluate to a logical value\n", remove_trailing_newline(expr));
    return 0;
  }

  return asLogical(result);
}
