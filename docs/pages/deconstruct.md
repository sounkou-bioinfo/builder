# Deconstruct

Builder supports destructuring assignment syntax for R, similar to JavaScript and Python.

Use the `.[var1, var2, ...]` syntax to extract multiple values from a list:

```r
.[x, my_var] <- call()
```

That will produce:

```r
x <- call()[[1]]
my_var <- call()[[2]]
```

Variables are assigned in order using list accessor `[[1]]`, `[[2]]`, etc. This makes it convenient to work with functions that return multiple values in a list.
