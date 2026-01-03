#ifndef FILE_H
#define FILE_H

typedef int(*Callback)(char *src, char *dst, Define **defs);

int exists(char *path);
char *strip_last_slash(char *path);
char *ensure_dir(char *path);
int walk(char *src_dir, char *dst_dir, Callback func, Define **defs);
int copy(char *src, char *dst, Define **defs);
int clean(char *src, char *dst, Define **defs);

#endif
