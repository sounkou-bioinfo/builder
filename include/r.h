#ifndef R_H
#define R_H

#include <Rinternals.h>
#include <R_ext/Parse.h>
#include <Rembedded.h>

int eval_if(char *expr);
void set_R_home();
char** extract_macro_args(const char *args_text, int *nargs);
char* extract_function_body(const char *func_text);

#endif
