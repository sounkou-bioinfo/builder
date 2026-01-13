#ifndef DEFINE_H
#define DEFINE_H

extern const char *DYNAMIC_DEFINITION;

typedef enum {
  DEF_VARIABLE,
  DEF_FUNCTION
} DefineType;

typedef struct {
    char **name;
    char **value;
    DefineType *type;
    int size;
    int capacity;
} Define;

Define *create_define();
void push(Define *arr, char *name, char *value, DefineType type);
void overwrite(Define **arr, char *name, char *value);
void push_builtins(Define *arr);
void free_array(Define *arr);
int define(Define **defines, char *line);
char *define_replace(Define **defines, char *line);
char *get_define_value(Define **defines, char *name);
void print_defines(Define *defines);
void *define_macro_init(char **macro);
char* str_replace(const char *orig, const char *find, const char *replace);

#endif
