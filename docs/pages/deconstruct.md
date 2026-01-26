---
title: Deconstruct
---

# Deconstruct

Builder supports destructuring assignment syntax for R, similar to JavaScript and Python.

Use the `.[var1, var2, ...]` syntax to extract multiple values from a list:

```r
.[x, my_var] <- call()
```

That will produce:

```r
.destructure_tmp_ <- call()
x <- .destructure_tmp_[[1]]
my_var <- .destructure_tmp_[[2]]
```

The expression on the right-hand side is evaluated once and stored in a hidden temporary variable (`.destructure_tmp_`). Each variable is then assigned from the corresponding index of that temporary. This ensures:

- Side effects only occur once
- Expensive computations aren't repeated
- Consistent behavior with functions that return different values on each call

Variables are assigned in order using list accessor `[[1]]`, `[[2]]`, etc. This makes it convenient to work with functions that return multiple values in a list.
