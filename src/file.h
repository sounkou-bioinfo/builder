#ifndef FILE_H
#define FILE_H

typedef int(*Callback)(char *src, char *dst, int n_extra_args, char **extra_args);

int transfer(char *src_dir, char *dst_dir, int n_extra_args, char **extra_args, Callback func);
int copy(char *src, char *dst, int n_extra_args, char **extra_args);
int clean(char *src, char *dst, int n_extra_args, char **extra_args);

#endif
