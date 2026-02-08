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

When enabled, Builder appends a `# filename:N` comment to the end of each line, indicating the source file and original line number.

## Example

**Input (srcr/utils.R):**

```r
#> define API_URL "https://api.example.com"

#> ifdef DEBUG
log_debug <- function(msg) {
  cat(msg, "\n")
}
#> endif

fetch_data <- function() {
  url <- API_URL
  httr::GET(url)
}
```

**Output (R/utils.R):**

```r
log_debug <- function(msg) { # srcr/utils.R:4
  cat(msg, "\n") # srcr/utils.R:5
} # srcr/utils.R:6

fetch_data <- function() { # srcr/utils.R:9
  url <- "https://api.example.com" # srcr/utils.R:10
  httr::GET(url) # srcr/utils.R:11
} # srcr/utils.R:12
```

Notice how:

- The `#> define` and `#> ifdef`/`#> endif` directives are removed from output
- Output has only 8 lines, but annotations reference the original source lines (4-6, 9-12)
- Each comment shows the source file and line number (e.g., `# srcr/utils.R:5`)

## Skipped Lines

The following lines do not receive sourcemap comments:

- Empty or whitespace-only lines
- Lines containing `#` (comments)
