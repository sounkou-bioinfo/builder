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

  Define *defines = create_define();

  get_definitions(defines, argc, argv);

  int must_clean = !has_arg(argc, argv, "-noclean");

  if(must_clean) {
    size_t msg_len = strlen("Cleaning: ") + strlen(output) + 1;
    char *msg = malloc(msg_len);
    if(msg == NULL) {
      log_error("Failed to allocate memory");
      return 1;
    }
    snprintf(msg, msg_len, "Cleaning: %s", output);
    log_info(msg);
    free(msg);
    walk(output, output, clean, NULL);
  } else {
    size_t msg_len = strlen("Not cleaning: ") + strlen(output) + 1;
    char *msg = malloc(msg_len);
    if(msg == NULL) {
      log_error("Failed to allocate memory");
      return 1;
    }
    snprintf(msg, msg_len, "Not cleaning: %s", output);
    log_info(msg);
    free(msg);
  }

  walk(input, output, copy, &defines);

  free(input);
  free(output);
  free_array(defines);

  // Terminate R embedded environment
  Rf_endEmbeddedR(0);

  return 0;
}
