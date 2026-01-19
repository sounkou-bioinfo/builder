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

### Another Example

```r
#define
TIME_IT(expr){
  start <- Sys.time()
  result <- expr
  cat("Time:", Sys.time() - start, "\n")
  result
}
#enddef

TIME_IT(slow_function())
```

See the full documentation for more features.
