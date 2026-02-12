
# builder

<!-- badges: start -->

[![R-CMD-check](https://github.com/devOpifex/builder/actions/workflows/cran-check.yml/badge.svg)](https://github.com/devOpifex/builder/actions/workflows/cran-check.yml)
<!-- badges: end -->

An R package shipping the [builder](https://builder.opifex.org)
preprocessor binary. Write R source with macros, conditionals, format
strings, destructuring, compile-time loops, and inline tests — builder
expands them into plain R before your package is built.

## Installation

### R package

``` r
# install.packages("remotes")
remotes::install_github("devOpifex/builder", subdir = "Rpkg")
```

### Standalone binaries

Pre-built binaries for Linux (amd64, arm64), macOS (arm64), and Windows
(amd64) are available from the
[r-pkg-release-binaries](https://github.com/devOpifex/builder/releases/tag/r-pkg-release-binaries)
release page. Binaries are built against specific R versions (e.g.,
`builder-linux-amd64-R4.5.2.tar.gz`) — pick the one matching your
installed R for ABI compatibility.

## Quick start

``` r
library(builder)
builder_version()
#> [1] "Builder v0.0.1"
```

Every example below writes source files into a temporary `srcr/`
directory, runs `builder_build()`, and sources the expanded output from
`R/`.

## Define & conditionals

`#> define` creates compile-time constants. `#> ifdef` / `#> ifndef` /
`#> endif` toggle code blocks.

``` r
out <- run_builder(c(
  '#> define VERSION "1.2.0"',
  '#> define DEBUG',
  '',
  'version <- VERSION',
  '',
  '#> ifdef DEBUG',
  'message("debug build")',
  '#> endif',
  '',
  '#> ifndef PRODUCTION',
  '.on <- TRUE',
  '#> endif'
), defines = c(PRODUCTION = ""))

show(out)
#> 
#> version <- "1.2.0"
```

The expanded output is valid R — constants are substituted and
conditional blocks resolved at build time.

``` r
source(textConnection(out$expanded), local = TRUE)
version
#> [1] "1.2.0"
```

## Macros

`#> macro` / `#> endmacro` defines a reusable function-like template.
Use `.arg` to paste the value, `..arg` to stringify it.

``` r
out <- run_builder(c(
  '#> macro',
  'ASSERT <- function(expr) {',
  '  if (!.expr) stop(..expr, " failed")',
  '}',
  '#> endmacro',
  '',
  'ASSERT(1 + 1 == 2)',
  'ASSERT(is.numeric(42))'
))

show(out)
#> 
#> if (!1 + 1 == 2) stop("1 + 1 == 2", " failed")
#> if (!is.numeric(42)) stop("is.numeric(42)", " failed")
```

``` r
source(textConnection(out$expanded), local = TRUE)
```

No error — both assertions passed.

## Format strings

`..FMT("text {expr}")` expands to `sprintf()`.

``` r
out <- run_builder(c(
  '#> define PKG "builder"',
  'x <- 42',
  'msg <- ..FMT("{PKG}: the answer is {x}")'
))

show(out)
#> x <- 42
#> msg <- sprintf('%s: the answer is %s', "builder", x)
```

``` r
source(textConnection(out$expanded), local = TRUE)
msg
#> [1] "builder: the answer is 42"
```

## Destructuring

`.[a, b] <- expr` unpacks a list into separate variables.

``` r
out <- run_builder(c(
  'f <- function() list(mean = 10, sd = 2)',
  '.[mu, sigma] <- f()'
))

show(out)
#> f <- function() list(mean = 10, sd = 2)
#> .destructure_tmp_ <- f()
#> mu <- .destructure_tmp_[[1]]
#> sigma <- .destructure_tmp_[[2]]
```

``` r
source(textConnection(out$expanded), local = TRUE)
c(mu = mu, sigma = sigma)
#>    mu sigma 
#>    10     2
```

## Compile-time for loops

`#> for i in 1:n` / `#> endfor` unrolls at build time.

``` r
out <- run_builder(c(
  '#> for i in 1:3',
  'col_..i.. <- mtcars[[..i..]]',
  '',
  '#> endfor'
))

show(out)
#> col_1 <- mtcars[[1]]
#> col_2 <- mtcars[[2]]
#> col_3 <- mtcars[[3]]
```

``` r
source(textConnection(out$expanded), local = TRUE)
head(col_1)
#> [1] 21.0 21.0 22.8 21.4 18.7 18.1
head(col_2)
#> [1] 6 6 4 6 8 6
```

## Expression conditionals

`#> if <expr>` evaluates an R expression at build time.

``` r
out <- run_builder(c(
  '#> if R.version$major >= 4',
  'pipe_op <- "|>"',
  '#> else',
  'pipe_op <- NULL',
  '#> endif'
))

show(out)
#> pipe_op <- "|>"
```

``` r
source(textConnection(out$expanded), local = TRUE)
pipe_op
#> [1] "|>"
```

## CLI usage from R

`builder_build()` accepts the same flags as the CLI.

``` r
# typical package workflow
builder_build(
  input = "srcr",
  output = "R",
  defines = c(DEBUG = "", VERSION = '"0.1.0"'),
  deadcode = TRUE,
  sourcemap = TRUE
)
```

Or call the binary directly for full control:

``` r
builder(c("-input", "srcr", "-output", "R", "-DDEBUG", "-deadcode"))
```

## License

GPL-2

## LLM Usage Disclose

Github copilot tools were use in helping write the files for
boostrapping this package. We read, edited and validated the changes.
