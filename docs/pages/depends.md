---
title: Depends
---

# Depends

Check that dev dependencies are installed before building. This is useful for ensuring development tools like testing frameworks, documentation generators, or linting packages are available.

## CLI Usage

```bash
builder -depends testthat devtools roxygen2
```

## Config Usage

```ini
depends: testthat devtools roxygen2
```

## Behavior

- Checks each package with `requireNamespace()`
- If any package is missing, build fails with an error
- All missing packages are reported before exiting

## Example

```bash
# Check dev dependencies before building
builder -depends testthat covr lintr

# Combined with other options
builder -input srcr/ -depends testthat devtools
```

This is intended for **development dependencies** - packages needed during the build process but not necessarily at runtime.
