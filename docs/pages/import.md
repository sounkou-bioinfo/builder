---
title: "Import"
---

# Import

Import `.rh` header files to share definitions, macros, and preflight checks across your project.

## Two Ways to Import

Use `#import` in your `.R` or `.rh` file:

```r
#import macros.rh
```

Or use the `-import` CLI flag:

```bash
builder -import macros.rh more-macros.rh -input srcr -output R
```

## What Can `.rh` Files Contain?

Header files support all builder directives:

- `#define` / `#enddef` - Macro definitions
- `#preflight` / `#endflight` - Validation checks (executed during build)
- `#ifdef` / `#ifndef` / `#endif` - Conditional compilation
- `#import` - Nested imports

## Processing Order

1. CLI imports are processed first (in order specified)
2. Inline `#import` directives are processed as encountered
3. Duplicate imports are automatically skipped
4. All imports are processed before source files

## Local Import

Import a local `.rh` file:

```r
#import utils.rh

foo <- function() {
  LOG("hello")
}
```

## Package Import

Import from an installed R package using `pkg::path` syntax. Package imports **must be namespaced** when used.
The file must be in the package's `inst` directory.

```r
#import mypkg::macros/utils.rh

foo <- function() {
  mypkg::LOG("hello")
  x <- mypkg::DEFAULT_VALUE
}
```

## Nested Imports

Header files can import other headers:

```r
// base.rh
#define VERSION 1.0.0

// utils.rh
#import base.rh
#define LOG(x) message(paste0("[v", VERSION, "] ", x))
```

## Preflight in Headers

Shared validation checks:

```r
// checks.rh
#preflight
if(!requireNamespace("rlang", quietly = TRUE)) {
  stop("rlang is required")
}
#endflight
```

## Summary

- `.rh` files support all directives (not just macros)
- Local imports: use definitions directly (`MACRO()`)
- Package imports: must namespace (`pkg::MACRO()`)
- Nested imports are supported and deduplicated
