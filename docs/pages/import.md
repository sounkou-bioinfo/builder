---
title: "Import"
---

# Import

Import `.rh` header files to share definitions, macros, and preflight checks across your project.

## Two Ways to Import

Use `#> import` in your `.R` or `.rh` file:

```r
#> import macros.rh
```

Or use the `-import` CLI flag:

```bash
builder -import macros.rh -input srcr -output R
```

## What Can `.rh` Files Contain?

Header files support all builder directives:

- `#> define` - Constants
- `#> macro` / `#> endmacro` - Macro definitions
- `#> preflight` / `#> endflight` - Validation checks (executed during build)
- `#> ifdef` / `#> ifndef` / `#> endif` - Conditional compilation
- `#> import` - Nested imports

## Local Import

Import a local `.rh` file:

```r
#> import utils.rh

foo <- function() {
  LOG("hello")
}
```

## Sharing Headers via Packages

You can distribute header files in R packages for reuse across projects.

### Package Structure

Place `.rh` files in your package's `inst/` directory:

```
mypkg/
├── DESCRIPTION
├── inst/
│   └── macros.rh
└── R/
    └── ...
```

### Importing from Packages

Use the `pkg::path` syntax to import from an installed package:

```r
#> import mypkg::macros.rh
```

Builder resolves this to the installed package's `inst/` directory (e.g., `/path/to/library/mypkg/macros.rh`).

### Subdirectories

You can organize headers in subdirectories:

```
inst/
├── macros/
│   ├── logging.rh
│   └── validation.rh
└── config.rh
```

```r
#> import mypkg::macros/logging.rh
#> import mypkg::config.rh
```

### Usage

Definitions from package imports work the same as local imports:

```r
#> import mypkg::macros.rh

foo <- function() {
  LOG("hello")
}
```

## Nested Imports

Header files can import other headers:

```r
# base.rh
#> define VERSION 1.0.0
```

```r
# utils.rh
#> import base.rh

#> macro
LOG <- function(x) {
  message(paste0("[v", VERSION, "] ", .x))
}
#> endmacro
```

## Preflight in Headers

Shared validation checks:

```r
# checks.rh
#> preflight
if(!requireNamespace("rlang", quietly = TRUE)) {
  stop("rlang is required")
}
#> endflight
```

## Processing Order

1. CLI imports are processed first (in order specified)
2. Inline `#> import` directives are processed as encountered
3. Duplicate imports are automatically skipped
4. All imports are processed before source files

## Summary

- `.rh` files support all directives (not just macros)
- Local imports: `#> import file.rh`
- Package imports: `#> import pkg::file.rh` (file at `inst/file.rh`)
- Nested imports are supported and deduplicated
