#ifndef __FSTRING_H__
#define __FSTRING_H__

typedef struct FString_t {
  char *value;
  struct FString_t *next;
} Fstring;

char *fstring_replace(char *str, int n);

#endif
