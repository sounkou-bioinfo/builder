#ifndef FSTRING_H
#define FSTRING_H

typedef struct FString_t {
  char *value;
  struct FString_t *next;
} Fstring;

char *fstring_replace(char *str, int n);

#endif
