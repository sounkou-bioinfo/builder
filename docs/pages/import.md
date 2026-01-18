# Import

Import `.rh` files containing macro definitions.

## Two Ways to Import

Use `#import` in your `.R` file:

```r
#import macros.rh
```

Or use the `-import` CLI flag:

```bash
builder -import macros.rh main.R
```

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

```r
#import mypkg::macros/utils.rh

foo <- function() {
  mypkg::LOG("hello")
  x <- mypkg::DEFAULT_VALUE
}
```

## Summary

- Only `.rh` files can be imported
- Local imports: use macros directly (`MACRO()`)
- Package imports: must namespace (`pkg::MACRO()`)
