#ifndef FILE_H
#define FILE_H

#include "define.h"
#include "parser.h"
#include "plugins.h"

struct RFile_t {
  char *src;
  char *dst;
  char *content;
  char *ns;
  struct RFile_t *next;
};

typedef struct RFile_t RFile;

typedef int(*Callback)(char *src, char *dst, Define **defs, Plugins *plugins);

int exists(char *path);
char *strip_last_slash(char *path);
char *ensure_dir(char *path);
int walk(char *src_dir, char *dst_dir, Callback func, Define **defs, Plugins *plugins);
int clean(char *src, char *dst, Define **defs, Plugins *plugins);
char *remove_leading_spaces(char *line);
int collect_files(RFile **files, char *src_dir, char *dst_dir);
int resolve_imports(RFile **files, Value *cli_imports);
int two_pass(RFile *files, Define **defs, Plugins *plugins, char *prepend, char *append, int deadcode);
void free_rfile(RFile *files);

#endif
