#ifndef DEFINE_H
#define DEFINE_H

typedef struct {
    char **name;
    char **value;
    int size;
    int capacity;
} Define;

Define *create_define();
void push(Define *arr, char *name, char *value);
void free_array(Define *arr);
void define(Define **defines, char *line);
char *define_replace(Define **defines, char *line);
char *get_define_value(Define **defines, char *name);
void print_defines(Define *defines);

#endif
