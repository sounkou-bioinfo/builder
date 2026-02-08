---
title: Format Strings
---

# Format Strings

Builder supports format strings for string interpolation using the `..FMT()` syntax.

## Basic Syntax

```r
x <- 1
y <- 2
print(..FMT("{x} + {y} = {x + y}"))
```

**Expands to:**

```r
x <- 1
y <- 2
print(sprintf('%s + %s = %s', x, y, x + y))
```

## Examples

### Variable Interpolation

```r
name <- "Alice"
age <- 30
print(..FMT("Hello {name}, you are {age} years old"))
```

### Expression Evaluation

Expressions inside `{}` are evaluated:

```r
x <- 10
print(..FMT("Double: {x * 2}, Square: {x ^ 2}"))
```

### Function Calls

```r
values <- c(1, 2, 3)
print(..FMT("Sum is {sum(values)}"))
```

### Quote Styles

Both single and double quotes are supported:

```r
..FMT("hello {name}")
..FMT('hello {name}')
```

## Limitations

- All values are formatted using `%s` (string conversion)
- No format specifiers (like Python's `{x:.2f}`)
