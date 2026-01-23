#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "parser.h"
#include "plugins.h"
#include "config.h"
#include "watch.h"
#include "file.h"
#include "log.h"
#include "r.h"

static int build(BuildContext *ctx)
{
  Define *defines = create_define();
  get_definitions(defines, ctx->argc, ctx->argv);

  if (ctx->must_clean) {
    printf("%s Cleaning: %s and testthat/\n", LOG_INFO, ctx->output);
    walk(ctx->output, ctx->output, clean, NULL, ctx->plugins);
  }

  RFile *files = NULL;
  int success = collect_files(&files, ctx->input, ctx->output);

  if (!success) {
    printf("%s Failed to collect files\n", LOG_ERROR);
    free_array(defines);
    return 1;
  }

  if (!resolve_imports(&files, ctx->imports)) {
    printf("%s Failed to resolve imports\n", LOG_ERROR);
    free_rfile(files);
    free_array(defines);
    return 1;
  }

  int result = two_pass(files, &defines, ctx->plugins, ctx->prepend, ctx->append, ctx->deadcode, ctx->sourcemap);

  free_rfile(files);
  free_array(defines);

  if (result) {
    printf("%s Failed to process files\n", LOG_ERROR);
    return 1;
  }

  return 0;
}

int main(int argc, char *argv[])
{
  set_R_home();

  char *r_argv[] = {"R", "--silent", "--no-save"};
  Rf_initEmbeddedR(3, r_argv);

  if (has_arg(argc, argv, "-help")) {
    printf("Usage: builder [OPTIONS]\n\n");
    printf("Options:\n");
    printf("  -input <path>         Input directory (default: srcr/)\n");
    printf("  -output <path>        Output directory (default: R/)\n");
    printf("  -noclean              Skip cleaning output directory before processing\n");
    printf("  -watch                Watch input directory and rebuild on changes\n");
    printf("  -D<NAME> <value>      Define directives, e.g.: -DDEBUG -DVALUE 42\n");
    printf("  -plugin               Use plugins, e.g.: -plugin pkg::plugin pkg::plugin2\n");
    printf("  -import               Import .rh files, e.g.: -import inst/main.rh pkg::main.rh\n");
    printf("  -prepend              Path to file to prepend to every output file (e.g.: license)\n");
    printf("  -append               Path to file to append to every output file\n");
    printf("  -deadcode             Enable dead variable/function detection\n");
    printf("  -sourcemap            Enable source map generation\n");
    printf("  -help                 Show this help message\n");
    printf("\n");
    printf("Example:\n");
    printf("  builder -input srcr/ -output R/ -DDEBUG\n");
    return 0;
  }

  BuildContext *cfg = NULL;
  if (has_config()) {
    cfg = get_config();
  }

  char *input = get_arg_value(argc, argv, "-input");
  if (input == NULL && cfg != NULL && cfg->input != NULL) {
    input = strdup(cfg->input);
  }
  if (input == NULL) {
    input = strdup("srcr/");
    printf("%s No -input, defaulting to srcr\n", LOG_WARNING);
  }
  if (input == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    free_config(cfg);
    return 1;
  }
  input = strip_last_slash(input);
  if (!exists(input)) {
    printf("%s Input directory does not exist\n", LOG_ERROR);
    free_config(cfg);
    free(input);
    return 1;
  }

  char *output = get_arg_value(argc, argv, "-output");
  if (output == NULL && cfg != NULL && cfg->output != NULL) {
    output = strdup(cfg->output);
  }
  if (output == NULL) {
    output = strdup("R/");
    printf("%s No -output, defaulting to R\n", LOG_WARNING);
  }
  if (output == NULL) {
    printf("%s Failed to allocate memory\n", LOG_ERROR);
    free_config(cfg);
    free(input);
    return 1;
  }
  if (!exists(output)) {
    printf("%s Output directory does not exist\n", LOG_ERROR);
    free_config(cfg);
    free(input);
    free(output);
    return 1;
  }
  output = ensure_dir(output);

  Value *imports = get_arg_values(argc, argv, "-import");
  if (imports == NULL && cfg != NULL && cfg->imports != NULL) {
    Value *current = cfg->imports;
    while (current != NULL) {
      imports = push_value(imports, strdup(current->name));
      current = current->next;
    }
  }

  Value *plugins_str = get_arg_values(argc, argv, "-plugin");
  if (plugins_str == NULL && cfg != NULL && cfg->plugins_str != NULL) {
    Value *current = cfg->plugins_str;
    while (current != NULL) {
      plugins_str = push_value(plugins_str, strdup(current->name));
      current = current->next;
    }
  }

  if (plugins_str != NULL) {
    printf("%s Using plugins:", LOG_INFO);
    Value *current = plugins_str;
    while (current != NULL) {
      printf(" %s", current->name);
      current = current->next;
    }
    printf("\n");
  }

  Plugins *plugins = plugins_init(plugins_str, input, output);
  free_value(plugins_str);

  int p_failed = plugins_failed(plugins);
  if (p_failed) {
    printf("%s Failed to initialize plugin(s) - stopping execution\n", LOG_ERROR);
    free_config(cfg);
    free(input);
    free(output);
    return 1;
  }

  char *prepend = get_arg_value(argc, argv, "-prepend");
  if (prepend == NULL && cfg != NULL && cfg->prepend != NULL) {
    prepend = strdup(cfg->prepend);
  }
  if (prepend != NULL) {
    printf("%s Prepending: %s\n", LOG_INFO, prepend);
  }

  char *append = get_arg_value(argc, argv, "-append");
  if (append == NULL && cfg != NULL && cfg->append != NULL) {
    append = strdup(cfg->append);
  }
  if (append != NULL) {
    printf("%s Appending: %s\n", LOG_INFO, append);
  }

  int deadcode = has_arg(argc, argv, "-deadcode");
  if (!deadcode && cfg != NULL) {
    deadcode = cfg->deadcode;
  }

  int sourcemap = has_arg(argc, argv, "-sourcemap");
  if (!sourcemap && cfg != NULL) {
    sourcemap = cfg->sourcemap;
  }

  int must_clean = 1;
  if (has_arg(argc, argv, "-noclean")) {
    must_clean = 0;
  } else if (cfg != NULL) {
    must_clean = cfg->must_clean;
  }

  int watch_mode = has_arg(argc, argv, "-watch");
  if (!watch_mode && cfg != NULL) {
    watch_mode = cfg->watch;
  }

  free_config(cfg);

  BuildContext ctx = {
    .argc = argc,
    .argv = argv,
    .input = input,
    .output = output,
    .imports = imports,
    .plugins_str = NULL,
    .prepend = prepend,
    .append = append,
    .deadcode = deadcode,
    .sourcemap = sourcemap,
    .must_clean = must_clean,
    .watch = watch_mode,
    .plugins = plugins
  };

  int result = 0;

  if (watch_mode) {
    printf("%s Watch mode enabled, monitoring %s\n", LOG_INFO, input);

    int fd = watch_init(input);
    if (fd == -1) {
      result = 1;
    } else {
      build(&ctx);
      ctx.must_clean = 0;

      while (1) {
        printf("%s Waiting for changes...\n", LOG_INFO);
        if (!watch_wait(fd)) break;
        printf("%s Change detected, rebuilding...\n", LOG_INFO);
        build(&ctx);
      }

      watch_close(fd);
    }
  } else {
    result = build(&ctx);
  }

  plugins_call(plugins, "end", NULL, NULL);
  free_plugins(plugins);
  free(prepend);
  free(append);
  free_value(imports);
  free(input);
  free(output);

  Rf_endEmbeddedR(0);

  return result;
}
