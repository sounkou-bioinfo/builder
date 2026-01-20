# Import

Import `.rh` files containing macro definitions.

## Two Ways to Import

Use `#import` in your `.R` file:

```r
#import macros.rh
```

Or use the `-import` CLI flag:

```bash
builder -import macros.rh more-macros.rh -input srcr -output R
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
Note that for this file to be accessible, the package must be installed and the file must be in the `inst` directory,
you can nest the file in subdirectories if you want, e.g: `mypkg::macros/utils.rh` (`inst/macros/utils.rh`).

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
