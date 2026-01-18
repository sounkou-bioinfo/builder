# Tests

Builder automatically generates testthat test files from inline `#test` blocks in your R source code. This allows you to keep tests close to the code they're testing while maintaining a clean separation in the output.

## How It Works

When Builder processes your source files, it:

- Detects `#test` blocks in your source code
- Extracts the test description and test expressions
- Generates testthat test files in `tests/testthat/`
- Automatically creates the directory structure if needed

## Syntax

Test blocks are defined using `#test` and `#endtest` directives:

```r
#test Description of what you're testing
expect_equal(my_function(), expected_value)
expect_true(some_condition)
#endtest
```

**Important notes:**

- The description comes on the same line as `#test`
- Test expressions are written as normal R code (no `#` prefix needed)
- Empty test blocks (no expressions) are skipped with a warning

## File Naming Convention

Generated test files follow this pattern:

```
tests/testthat/test-builder-<filename>
```

**Examples:**

- `srcr/main.R` → `tests/testthat/test-builder-main.R`
- `srcr/sub/utils.R` → `tests/testthat/test-builder-sub-utils.R`

Subdirectory paths are flattened using hyphens to keep all test files in a single directory.

## Example

**Input file (srcr/main.R):**

```r
foo <- function() {
  return(1)
}

#test That it returns one!
expect_equal(foo(), 1)
expect_type(foo(), "double")
#endtest
```

**Command:**

```bash
./builder -input srcr -output R
```

**Generated test file (tests/testthat/test-builder-main.R):**

```r
test_that("That it returns one!", {
  expect_equal(foo(), 1)
  expect_type(foo(), "double")
})
```

## Multiple Tests

You can have multiple test blocks in a single source file:

```r
add <- function(a, b) {
  return(a + b)
}

#test Addition works correctly
expect_equal(add(2, 3), 5)
expect_equal(add(-1, 1), 0)
#endtest

multiply <- function(a, b) {
  return(a * b)
}

#test Multiplication works correctly
expect_equal(multiply(2, 3), 6)
expect_equal(multiply(0, 5), 0)
#endtest
```

All tests from the same source file are collected into a single test file with multiple `test_that()` blocks.

## Cleaning Test Files

When Builder cleans the output directory (default behavior), it also automatically removes the corresponding test files. This ensures a fresh build each time.

```bash
# Default: cleans R/ output and tests/testthat/ test files
./builder -input srcr -output R

# Skip cleaning with -noclean flag
./builder -input srcr -output R -noclean
```

## Integration with testthat

The generated test files are standard testthat format and can be run using:

```r
# In R console
library(testthat)
test_dir("tests/testthat")
```

Or with the standard R CMD check workflow for package development.

## Best Practices

- **Keep tests close to code** - Write tests right after the functions they test
- **Use descriptive names** - Make test descriptions clear and specific
- **One concern per test** - Test one specific behavior per test block
- **Include edge cases** - Test boundary conditions and error cases

## Limitations

- Test expressions cannot span multiple lines without proper R continuation
- Tests are removed from the processed R output
