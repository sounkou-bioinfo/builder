#ifndef PARSER_H
#define PARSER_H

char *get_arg_value(int argc, char *argv[], char *arg);
int has_arg(int argc, char *argv[], char *arg);
int get_extra_args(char **buffer, int argc, char *argv[]);

#endif
