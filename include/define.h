#ifndef DEFINE_H
#define DEFINE_H

extern const char *DYNAMIC_DEFINITION;

typedef struct {
    char **name;
    char **value;
    int size;
    int capacity;
} Define;

Define *create_define();
void push(Define *arr, char *name, char *value);
void push_builtins(Define *arr);
void free_array(Define *arr);
void define(Define **defines, char *line);
char *define_replace(Define **defines, char *line, int line_number, char *src);
char *get_define_value(Define **defines, char *name);
void print_defines(Define *defines);

#endif
