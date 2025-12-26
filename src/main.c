#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "define.h"
#include "log.h"
#include "file.h"

int main(int argc, char *argv[])
{
  int help = has_arg(argc, argv, "-help");

  if(help) {
    printf("Usage: builder [OPTIONS]\n\n");
    printf("Options:\n");
    printf("  -input <path>         Input directory (default: srcr/)\n");
    printf("  -output <path>        Output directory (default: R/)\n");
    printf("  -noclean              Skip cleaning output directory before processing\n");
    printf("  -D<NAME>              Define boolean\n");
    printf("  -help                 Show this help message\n");
    printf("\n");
    printf("Example:\n");
    printf("  builder -input src/ -output build/ -DDEBUG\n");
    return 0;
  }

  char *input = get_arg_value(argc, argv, "-input");

  if(input == NULL) {
    input = strdup("srcr/");
    log_info("No -input, defaulting to srcr");
  }

  if(input == NULL) {
    log_error("Failed to allocate memory");
    return 1;
  }

  input = strip_last_slash(input);

  if(!exists(input)) {
    log_error("Input directory does not exist");
    return 1;
  }

  char *output = get_arg_value(argc, argv, "-output");

  if(output == NULL) {
    output = strdup("R/");
    log_info("No -output, defaulting to R");
  }
  
  if(output == NULL) {
    log_error("Failed to allocate memory");
    return 1;
  }

  if(!exists(output)) {
    log_error("Output directory does not exist");
    return 1;
  }

  output = ensure_dir(output);

  char **buffer = malloc(sizeof(char*) * argc);
  if(buffer == NULL) {
    log_error("Failed to allocate memory");
    return 1;
  }

  int n_extra = get_extra_args(buffer, argc, argv);

  int must_clean = !has_arg(argc, argv, "-noclean");

  if(must_clean) {
    char *msg = strdup("Cleaning: ");
    strcat(msg, output);
    log_info(msg);
    free(msg);
    walk(output, output, n_extra, buffer, clean, NULL);
  } else {
    char *msg = strdup("Not cleaning: ");
    strcat(msg, output);
    log_info(msg);
    free(msg);
  }

  Define *defines = create_define();
  walk(input, output, n_extra, buffer, copy, &defines);

  free(buffer);
  free(input);
  free(output);
  free_array(defines);

  return 0;
}
