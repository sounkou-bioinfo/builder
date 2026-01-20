# Dead Code Analysis

Builder can detect unused variables and functions in your R package with the `-deadcode` flag. This helps identify code that may be safe to remove or indicates potential bugs.

⚠️ Warning: this is the only feature that is vibe-coded to some extent and static analysis of R code is 
notoriously difficult given the capabilities of the language, these are the reasons this 
feature is not enabled by default.

## Usage

```bash
builder -input srcr -output R -deadcode
```

When enabled, Builder performs a two-pass analysis after preprocessing:

1. **Pass 1**: Collects all variable and function declarations across all files
2. **Pass 2**: Marks variables and functions as used when they are referenced

After both passes, any declarations that were never referenced are reported as warnings.

## Example

**Input files:**

```r
# srcr/utils.R
helper_used <- function(x) {
  x + 1
}

helper_unused <- function(x) {
  x * 2
}
```

```r
# srcr/main.R
result <- helper_used(5)
print(result)
```

**Command:**

```bash
builder -input srcr -output R -deadcode
```

**Output:**

```
[INFO] Running dead code analysis...
[WARNING] Unused function 'helper_unused' at line 2 in R/utils.R
[WARNING] Found 1 unused variable(s)/function(s)
```

## Cross-File Detection

The analysis works across all files in your package. A function defined in one file and used in another will be correctly identified as used:

```r
# File A: defines foo()
foo <- function() { 42 }

# File B: uses foo()
x <- foo()
```

In this case, `foo` is not flagged because it's used in File B.

## Nested Scopes

Builder respects R's lexical scoping rules. Variables in nested functions can reference outer scopes:

```r
outer <- function() {
  helper <- function(x) { x + 1 }  # Used below
  unused <- 42                      # Flagged as unused
  
  helper(10)
}
```

## Exclusions

Certain names are automatically excluded from dead code detection:

- Names starting with `.` (e.g., `.onLoad`, `.data`)
- Names starting with `_`
- R special functions: `.onLoad`, `.onUnload`, `.onAttach`, `.onDetach`, `.Last.lib`, `.First.lib`

These are typically package lifecycle functions or internal variables that may not have explicit callers in user code.

## Limitations

- **Exported functions**: Currently does not check NAMESPACE exports. Exported functions that are only called externally will be flagged. Consider these warnings as informational.
- **Dynamic calls**: Uses of `do.call()`, `get()`, or other dynamic dispatch mechanisms cannot be tracked statically.
- **S3/S4 methods**: Method dispatch may cause false positives for methods that are called indirectly.

## Best Practices

- Run dead code analysis periodically during development
- Review warnings carefully before removing code
- Use exclusion patterns (names starting with `.`) for intentionally unused internal variables
- Exported public API functions may appear unused - verify against NAMESPACE before removing
