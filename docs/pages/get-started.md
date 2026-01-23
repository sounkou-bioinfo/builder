---
title: Get Started
---

# Get Started

This guide covers the basics of builder: packages, directives, and macros.

## Package

Builder reads R files from the `srcr` directory (default) and writes to the `R` directory (default).

```bash
./builder -input srcr -output R
```

This allows organising your R package source in **nested directories**:

```bash
srcr
├── main.R
└── utils
    └── helpers.R
```

Produces:

```bash
R
├── main.R
└── utils-helpers.R
```

Subdirectory paths are flattened with hyphens.

## Directives

Builder supports C-like preprocessor directives.

### Define Constants

```r
#define DEBUG TRUE
#define VERSION 2

x <- DEBUG
cat("Version:", VERSION)
```

### Conditional Compilation

```r
#ifdef DEBUG
cat("Debug mode\n")
#endif

#if VERSION > 1
cat("New version\n")
#else
cat("Old version\n")
#endif
```

### Command Line Definitions

Pass definitions at build time:

```bash
./builder -input srcr -output R -DDEBUG -DVERSION 3
```

Command-line definitions override file-based `#define` directives.

## Macros

Macros are function-like directives with parameters.

```r
#define
LOG(level, msg){
  cat("[", level, "] ", msg, "\n", sep = "")
}
#enddef

LOG("INFO", "Started")
```

Expands to:

```r
cat("[", "INFO", "] ", "Started", "\n", sep = "")
```

### Syntax

- Start with `#define` alone on a line
- Macro signature: `NAME(arg1, arg2, ...){`
- Body enclosed in curly braces
- End with `#enddef`

## Usage

You can configure builder in two ways:

### Config File

Create a `builder.ini` file in your project root:

```ini
input: srcr/
output: R/
```

Then simply run:

```bash
builder
```

See [Configuration File](/config) for all options.

### Build Scripts

Alternatively, place your builder call in a `Makefile`, `build.R` or `build.sh` script:

```r
#!/usr/bin/env Rscript

system2("builder", c("-input", "srcr", "-output", "R"))
devtools::document()
devtools::check()
```

See the full documentation for more features.
