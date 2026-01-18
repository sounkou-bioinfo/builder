# f-strings

Builder accepts f-strings

```r
x <- 1
y <- 2
print(f'{x} + {y} = {x + y}')
```

That will produce:

```r
x <- 1
y <- 2
print(sprintf('%s + %s = %s', x, y, x + y))
```

Note that it's currently simple in its implementation and just uses `%s` formats.
