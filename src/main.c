#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "parser.h"
#include "import.h"
#include "plugins.h"
#include "file.h"
#include "log.h"
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
    printf("  -plugins              Use plugins, e.g.: -plugins pkg::plugin pkg::plugin2\n");
    printf("  -import               Import .rh files, e.g.: -imports inst/main.rh pkg::main.rh\n");
    printf("  -help                 Show this help message\n");
    printf("\n");
    printf("Example:\n");
    printf("  builder -input srcr/ -output R/ -DDEBUG\n");
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
    printf("%s No -output, defaulting to R\n", LOG_WARNING);
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
  get_definitions(defines, argc, argv);

  Value *imports = get_arg_values(argc, argv, "-import");
  if(imports != NULL) {
    printf("%s Importing header files:", LOG_INFO);
    Value *current = imports;
    while(current != NULL) {
      printf(" %s", current->name);
      current = current->next;
    }
    printf("\n");
  }
  import_defines(&defines, imports);

  Value *plugins_str = get_arg_values(argc, argv, "-plugins");

  if(plugins_str != NULL) {
    printf("%s Using plugins:", LOG_INFO);
    Value *current = plugins_str;
    while(current != NULL) {
      printf(" %s", current->name);
      current = current->next;
    }
    printf("\n");
  }

  Plugins *plugins = plugins_init(plugins_str, input, output);
  int p_failed = plugins_failed(plugins);
  if(p_failed) {
    printf("%s Failed to initialize plugin(s) - stopping execution\n", LOG_ERROR);
    return 1;
  }
  int must_clean = !has_arg(argc, argv, "-noclean");

  if(must_clean) {
    printf("%s Cleaning: %s and testthat/\n", LOG_INFO, output);
    walk(output, output, clean, NULL, plugins);
  } else {
    printf("%s Not cleaning: %s\n", LOG_INFO, output);
  }

  walk(input, output, copy, &defines, plugins);
  plugins_call(plugins, "end", NULL);

  free(input);
  free(output);
  free_array(defines);

  // Terminate R embedded environment
  Rf_endEmbeddedR(0);

  return 0;
}
