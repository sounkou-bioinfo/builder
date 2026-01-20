#ifndef DEADCODE_H
#define DEADCODE_H

#include <Rinternals.h>
#include "file.h"

typedef struct Binding {
    char *name;
    int is_function;
    int is_used;
    int line;
    char *file;
    struct Binding *next;
} Binding;

typedef struct Environment {
    Binding *bindings;
    struct Environment *parent;
    int is_global;
} Environment;

Environment* env_create(Environment *parent);
void env_free(Environment *env);
void env_define(Environment *env, const char *name, int is_func, int line, const char *file);
void env_mark_used(Environment *env, const char *name);
int is_excluded_name(const char *name);

int analyse_deadcode(RFile *files);

#endif
