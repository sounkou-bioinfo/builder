#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "define.h"
#include "log.h"
#include "file.h"
#include "r.h"

int main(int argc, char *argv[])
{
  set_R_home();

  // Initialize R embedded environment
  char *r_argv[] = {"R", "--silent", "--no-save"};
  Rf_initEmbeddedR(3, r_argv); 

  int help = has_arg(argc, argv, "-help");

  if(help) {
    printf("Usage: builder [OPTIONS]\n\n");
    printf("Options:\n");
    printf("  -input <path>         Input directory (default: srcr/)\n");
    printf("  -output <path>        Output directory (default: R/)\n");
    printf("  -noclean              Skip cleaning output directory before processing\n");
    printf("  -D<NAME>              Define directives, e.g.: -DDEBUG -DVALUE 42\n");
    printf("  -help                 Show this help message\n");
    printf("\n");
    printf("Example:\n");
    printf("  builder -input src/ -output build/ -DDEBUG\n");
    return 0;
  }

  char *input = get_arg_value(argc, argv, "-input");

  if(input == NULL) {
    input = strdup("srcr/");
    printf("%s No -input, defaulting to srcr\n", LOG_INFO);
  }

  if(input == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return 1;
  }

  input = strip_last_slash(input);

  if(!exists(input)) {
    printf("%s Input directory does not exist\n", LOG_ERROR);
    return 1;
  }

  char *output = get_arg_value(argc, argv, "-output");

  if(output == NULL) {
    output = strdup("R/");
    printf("%s No -output, defaulting to R\n", LOG_INFO);
  }
  
  if(output == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    return 1;
  }

  if(!exists(output)) {
    printf("%s Output directory does not exist\n", LOG_ERROR);
    return 1;
  }

  output = ensure_dir(output);

  Define *defines = create_define();
  push_builtins(defines);
  get_definitions(defines, argc, argv);

  int must_clean = !has_arg(argc, argv, "-noclean");

  if(must_clean) {
    printf("%s Cleaning: %s\n", LOG_INFO, output);
    walk(output, output, clean, NULL);
  } else {
    printf("%s Not cleaning: %s\n", LOG_INFO, output);
  }

  walk(input, output, copy, &defines);

  free(input);
  free(output);
  free_array(defines);

  // Terminate R embedded environment
  Rf_endEmbeddedR(0);

  return 0;
}
