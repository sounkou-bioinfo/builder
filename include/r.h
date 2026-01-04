#ifndef R_H
#define R_H

#include <Rinternals.h>
#include <R_ext/Parse.h>
#include <Rembedded.h>

int eval_if(char *expr);
void set_R_home();

#endif
