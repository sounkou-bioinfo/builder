---
title: "Headers"
---

# Header Files (.rh)

Header files (`.rh`) are preprocessor-only files used to share macros, constants, and build-time checks across your project. Unlike `.R` files, headers should not contain R code intended for runtime execution.

## Purpose

- Share macro definitions across multiple source files
- Define constants used throughout your project
- Centralize preflight checks that validate your build environment
- Keep preprocessor logic separate from application code

## What Headers Should Contain

Headers are meant for **preprocessor directives only**:

| Directive | Purpose |
|-----------|---------|
| `#define NAME value` | Constants |
| `#define` / `#enddef` | Macro definitions |
| `#preflight` / `#endflight` | Build-time validation |
| `#ifdef` / `#ifndef` / `#if` / `#endif` | Conditional compilation |
| `#import` | Nested header imports |

## What Headers Should NOT Contain

Headers should **not** contain regular R code:

```r
// BAD: This belongs in a .R file, not a header
my_function <- function(x) {
  x + 1
}

data <- read.csv("file.csv")
```

The rationale: `.rh` files are processed by the preprocessor to extract definitions. Any R code outside of `#preflight` blocks would be silently ignored or cause unexpected behavior. Keep runtime code in `.R` files.

The one exception is `#preflight` blocks, which contain R code that runs **at build time** to validate your environment:

```r
#preflight
if (!requireNamespace("rlang", quietly = TRUE)) {
  stop("rlang is required to build this project")
}
#endflight
```

## Example Header

A typical header file might look like:

```r
// config.rh - Project-wide configuration

// Version info
#define VERSION "1.2.0"
#define VERSION_MAJOR 1
#define VERSION_MINOR 2

// Feature flags
#define ENABLE_LOGGING
#define MAX_RETRIES 3

// Validate build environment
#preflight
if (getRversion() < "4.0.0") {
  stop("R >= 4.0.0 is required")
}
#endflight

// Utility macros
#define
LOG(msg){
  message(paste0("[", Sys.time(), "] ", msg))
}
#enddef

#define
ASSERT(cond, msg){
  if (!cond) stop(msg)
}
#enddef
```

## Importing Headers

Use `#import` in your `.R` files or pass headers via the `-import` CLI flag. See [Import](import.html) for details on local imports, package imports, and nested imports.

```r
#import config.rh

LOG("Application starting, version " %+% VERSION)
```
