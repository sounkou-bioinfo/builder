#ifndef PARSER_H
#define PARSER_H

void get_arg_value(char **value, int argc, char *argv[], char *arg);
int has_arg(int argc, char *argv[], char *arg);
int get_extra_args(char **buffer, int argc, char *argv[]);

#endif
