#ifndef PARSER_H
#define PARSER_H

#include "define.h"

extern const char *NO_DEFINITION;

char *get_arg_value(int argc, char *argv[], char *arg);
int has_arg(int argc, char *argv[], char *arg);
void get_definitions(Define *arr, int argc, char *argv[]);

#endif
