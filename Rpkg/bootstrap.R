#!/usr/bin/env Rscript
# Bootstrap script: copies builder C sources into inst/builder_src/
# Run from Rpkg/:  Rscript bootstrap.R
# Or with explicit root: Rscript bootstrap.R /path/to/builder

args <- commandArgs(trailingOnly = TRUE)
repo_root <- if (length(args) >= 1) args[1] else NULL

source(file.path("R", "bootstrap.R"))

builder_bootstrap(repo_root = repo_root)
