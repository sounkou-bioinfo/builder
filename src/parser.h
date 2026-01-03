#ifndef PARSER_H
#define PARSER_H

#include "define.h"

char *get_arg_value(int argc, char *argv[], char *arg);
int has_arg(int argc, char *argv[], char *arg);
void get_definitions(Define *arr, int argc, char *argv[]);

#endif
