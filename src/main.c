#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "log.h"

int main(int argc, char *argv[])
{
  int must_clean = has_arg(argc, argv, "-noclean");

  if(must_clean) {
    log_info("Cleaning...");
  } else {
    log_info("Not cleaning...");
  }

  char *input = NULL;
  get_arg_value(&input, argc, argv, "-input");
  log_info(input);

  char **buffer = malloc(sizeof(char*) * argc);
  int nextra = get_extra_args(buffer, argc, argv);

  for (int i = 0; i < nextra; i++)
  {
    log_info(buffer[i]);
  }

  free(buffer);

  return 0;
}
