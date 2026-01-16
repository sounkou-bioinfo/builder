#ifndef FILE_H
#define FILE_H

#include "define.h"
#include "parser.h"

typedef int(*Callback)(char *src, char *dst, Define **defs, Value *plugins);

int exists(char *path);
char *strip_last_slash(char *path);
char *ensure_dir(char *path);
int walk(char *src_dir, char *dst_dir, Callback func, Define **defs, Value *plugins);
int copy(char *src, char *dst, Define **defs, Value *plugins);
int clean(char *src, char *dst, Define **defs, Value *plugins);
char *remove_leading_spaces(char *line);

#endif
