---
title: Source Map
---

# Source Map

Builder can add line number comments to your output files with the `-sourcemap` flag. This helps trace errors in processed R files back to their original source line numbers.

## Usage

```bash
builder -input srcr -output R -sourcemap
```

## How It Works

When enabled, Builder appends a `# line: N` comment to the end of each line, indicating the original line number from the source file.

## Example

**Input (srcr/main.R):**

```r
foo <- function(x) {
  x + 1
}

# a comment
bar <- 42
```

**Output (R/main.R):**

```r
foo <- function(x) { # line: 1
  x + 1 # line: 2
} # line: 3

# a comment
bar <- 42 # line: 6
```

## Skipped Lines

The following lines do not receive sourcemap comments:

- Empty or whitespace-only lines
- Lines containing `#` (comments)

## Limitations

- Line numbers reference the source file lines, but if macros or includes expand content, the mapping reflects the pre-expanded line numbers
