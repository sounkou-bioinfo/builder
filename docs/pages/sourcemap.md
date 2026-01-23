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

**Input (srcr/utils.R):**

```r
#define API_URL "https://api.example.com"

#ifdef DEBUG
log_debug <- function(msg) {
  cat(msg, "\n")
}
#endif

fetch_data <- function() {
  url <- API_URL
  httr::GET(url)
}
```

**Output (R/utils.R):**

```r
log_debug <- function(msg) { # line: 4
  cat(msg, "\n") # line: 5
} # line: 6

fetch_data <- function() { # line: 9
  url <- "https://api.example.com" # line: 10
  httr::GET(url) # line: 11
} # line: 12
```

Notice how:

- The `#define` and `#ifdef`/`#endif` directives are removed from output
- Output has only 8 lines, but annotations reference the original source lines (4-6, 9-12)
- If R reports an error on "line 5 of R/utils.R", you know to check **line 5 of srcr/utils.R**

## Skipped Lines

The following lines do not receive sourcemap comments:

- Empty or whitespace-only lines
- Lines containing `#` (comments)
