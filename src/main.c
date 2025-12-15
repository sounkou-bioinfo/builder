#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "log.h"

int main(int argc, char *argv[])
{
  int must_clean = !has_arg(argc, argv, "-noclean");

  if(must_clean) {
    log_info("Cleaning...");
  } else {
    log_info("Not cleaning...");
  }

  char *input = get_arg_value(argc, argv, "-input");

  if(input == NULL) {
    input = strdup("srcr");
    log_info("No -input, defaulting to srcr");
  }
  if(input == NULL) {
    log_error("Failed to allocate memory");
    return 1;
  }
  log_info(input);

  char *output = get_arg_value(argc, argv, "-output");

  if(output == NULL) {
    output = strdup("R");
    log_info("No -output, defaulting to R");
  }
  if(output == NULL) {
    log_error("Failed to allocate memory");
    return 1;
  }
  log_info(output);

  char **buffer = malloc(sizeof(char*) * argc);
  if(buffer == NULL) {
    log_error("Failed to allocate memory");
    return 1;
  }

  int nextra = get_extra_args(buffer, argc, argv);

  log_info("Directives:");
  for (int i = 0; i < nextra; i++)
  {
    log_info(buffer[i]);
  }

  free(buffer);
  free(input);
  free(output);

  return 0;
}
