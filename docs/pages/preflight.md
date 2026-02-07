---
title: Preflight
---

# Preflight

Preflight blocks allow you to run R code **before** the main preprocessing phase. This is useful for validation checks, environment verification, or early-exit conditions that should halt the build if requirements aren't met.

## How It Works

When Builder encounters a `#> preflight` block during processing, it:

- Extracts all R code between `#> preflight` and the end marker
- Immediately evaluates the R code
- If the code fails (throws an error), the build stops
- If successful, preprocessing continues normally

The preflight code is executed as-is and is **not** subject to preprocessor directives or macro expansion.

## Syntax

Preflight blocks are defined using `#> preflight` and closed with either `#> endpreflight` or the shorthand `#> endflight`:

```r
#> preflight
# Your R validation code here
if(!condition) stop("Requirement not met")
#> endflight
```

## Example: Version Check

A common use case is verifying the R version meets minimum requirements:

```r
#> preflight
rver <- version$major |>
  as.integer()

if(rver < 4)
  stop("We need R version 4.x.x at least")
#> endflight

# Rest of your code continues here...
foo <- function() {
  return(1)
}
```

If the R version is less than 4, the build will halt immediately with the error message. Otherwise, preprocessing continues normally.

## Example: Package Check

You can verify required packages are available before building:

```r
#> preflight
required <- c("dplyr", "ggplot2", "tidyr")
missing <- required[!sapply(required, requireNamespace, quietly = TRUE)]

if(length(missing) > 0)
  stop("Missing required packages: ", paste(missing, collapse = ", "))
#> endpreflight
```

## Example: Environment Check

Validate environment variables or system requirements:

```r
#> preflight
api_key <- Sys.getenv("API_KEY")
if(nchar(api_key) == 0)
  stop("API_KEY environment variable must be set")

if(.Platform$OS.type != "unix")
  warning("This package is optimized for Unix systems")
#> endflight
```

## End Markers

Two end markers are supported:

- `#> endpreflight` - The formal closing tag
- `#> endflight` - A convenient shorthand

Both are functionally equivalent.

## Behavior on Failure

When preflight code fails:

- The error message is displayed
- Builder exits with a non-zero status code
- No output files are generated

This makes preflight checks ideal for CI/CD pipelines where you want builds to fail fast if requirements aren't met.

## Placement

Preflight blocks can appear anywhere in your source file, but are typically placed at the top before any other code. The block is processed when encountered during the sequential file reading.

## Limitations

- Preflight code is not preprocessed (no macros, no `#> define` substitution)
- The code block must be valid R syntax on its own
- Output from preflight code is not captured in any output files
