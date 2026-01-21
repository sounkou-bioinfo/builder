---
title: F-strings
---

# F-strings

Builder supports Python-style f-strings for string interpolation.

**Important:** Only single quotes are supported (`f'...'`), not double quotes.

## Basic Syntax

```r
x <- 1
y <- 2
print(f'{x} + {y} = {x + y}')
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
print(f'Hello {name}, you are {age} years old')
```

### Expression Evaluation

Expressions inside `{}` are evaluated:

```r
x <- 10
print(f'Double: {x * 2}, Square: {x ^ 2}')
```

### Function Calls

```r
values <- c(1, 2, 3)
print(f'Sum is {sum(values)}')
```

## Limitations

- Only single quotes (`f'...'`) are supported
- All values are formatted using `%s` (string conversion)
- No format specifiers (like Python's `{x:.2f}`)
