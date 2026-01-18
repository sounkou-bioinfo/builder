# Macros

Macros are function-like preprocessor directives that allow you to define reusable code templates with parameters. Unlike simple `#define` replacements, macros can accept arguments and expand into multiple lines of code.

## Advantages

- **Easy metaprogramming:** Write code that generates code, enabling powerful abstractions and code generation patterns.
- **Zero overhead:** Macros have no runtime performance cost since they're expanded at compile time.
- **Compile time expansion:** All macro processing happens during preprocessing, before your code executes.

## Basic Syntax

Macros are defined using `#define` on its own line, followed by the macro signature and body. The macro ends with `#enddef`.

**Syntax:**

```r
#define
MACRO_NAME(arg1, arg2, ...){
  body
}
#enddef
```

```r
#define
LOG_EVAL(expr){
  cat("Evaluating stuff\n")
  expr
}
#enddef

LOG_EVAL(sum(1, 1, 1))
```

**Expands to:**

```r
cat("Evaluating stuff\n")
sum(1, 1, 1)
```

## Multiline Macro Bodies

Macro bodies can contain multiple lines of R code with full syntax highlighting support in your editor.

```r
#define
DEBUG_BLOCK(var, message){
  cat("Debug:", message, "\n")
  cat("Value is", var, "\n")
  cat("Type:", typeof(var), "\n")
}
#enddef

x <- 42
DEBUG_BLOCK(x, "Checking variable")
```

**Expands to:**

```r
x <- 42
cat("Debug:", "Checking variable", "\n")
cat("Value is", x, "\n")
cat("Type:", typeof(x), "\n")
```

## Parameter Substitution

Macro parameters are replaced throughout the macro body when the macro is invoked. All occurrences of the parameter name will be substituted with the corresponding argument.

```r
#define
REPEAT(n, action){
  for (i in 1:n) {
    action
  }
}
#enddef

REPEAT(3, print("Hello"))
```

**Expands to:**

```r
for (i in 1:3) {
  print("Hello")
}
```

## Macros with Multiple Arguments

Macros can accept multiple arguments, which are separated by commas in both the definition and invocation.

```r
#define
VALIDATE(value, min, max){
  if (value < min || value > max) {
    stop("Value out of range: ", value)
  }
}
#enddef

x <- 15
VALIDATE(x, 0, 100)
```

**Expands to:**

```r
x <- 15
if (x < 0 || x > 100) {
  stop("Value out of range: ", x)
}
```

## Practical Examples

### Logging Macro

```r
#define
LOG(level, msg){
  cat("[", level, "] ", msg, "\n", sep = "")
}
#enddef

LOG("INFO", "Application started")
LOG("ERROR", "Something went wrong")
```

**Expands to:**

```r
cat("[", "INFO", "] ", "Application started", "\n", sep = "")
cat("[", "ERROR", "] ", "Something went wrong", "\n", sep = "")
```

### Try-Catch Wrapper

```r
#define
TRY_CATCH(code, error_msg){
  tryCatch({
    code
  }, error = function(e) {
    cat(error_msg, ":", e$message, "\n")
  })
}
#enddef

TRY_CATCH(risky_operation(), "Operation failed")
```

**Expands to:**

```r
tryCatch({
  risky_operation()
}, error = function(e) {
  cat("Operation failed", ":", e$message, "\n")
})
```

### Timing Macro

```r
#define
TIME_IT(expr){
  start <- Sys.time()
  result <- expr
  end <- Sys.time()
  cat("Execution time:", end - start, "\n")
  result
}
#enddef

TIME_IT(slow_computation())
```

**Expands to:**

```r
start <- Sys.time()
result <- slow_computation()
end <- Sys.time()
cat("Execution time:", end - start, "\n")
result
```

## Important Notes

- Macros start with `#define` alone on a line
- The macro signature and body follow on subsequent lines
- Macro bodies must be enclosed in curly braces `{}`
- Macros end with `#enddef`
- Macros support multiline definitions and can span many lines (up to 1024 lines)
- Macro parameters are simple text substitution - be careful with expressions that might be evaluated multiple times
- Simple `#define NAME value` constants still work as before
