---
title: Macros
---

# Macros

Macros are function-like preprocessor directives that allow you to define reusable code templates with parameters. Unlike simple `#define` replacements, macros can accept arguments and expand into multiple lines of code.

## Advantages

- **Easy metaprogramming:** Write code that generates code, enabling powerful abstractions and code generation patterns.
- **Zero overhead:** Macros have no runtime performance cost since they're expanded at compile time.
- **Compile time expansion:** All macro processing happens during preprocessing, before your code executes.

## Basic Syntax

Macros are defined using `#macro` on its own line, followed by the macro signature and body. The macro ends with `#endmacro`.

**Syntax:**

```r
#macro
MACRO_NAME(arg1, arg2, ...){
  body using .arg1, .arg2, etc.
}
#endmacro
```

## Global vs Local Macros

By default, macros are **global** - their expanded code is inserted directly without wrapping.

Use `#macro local` to create a **local** macro that is wrapped in `local({...})`. This prevents the macro from polluting the calling environment, avoiding issues like accidentally overwriting variables.

### Global Macro (Default)

```r
#macro
SETUP_ENV(name){
  .name_env <- new.env()
  .name_data <- list()
}
#endmacro

SETUP_ENV(app)
```

**Expands to:**

```r
app_env <- new.env()
app_data <- list()
```

### Local Macro

```r
#macro local
SWAP(a, b){
  tmp <- .a
  .a <- .b
  .b <- tmp
}
#endmacro

SWAP(x, y)
```

**Expands to:**

```r
local({
  tmp <- x
  x <- y
  y <- tmp
})
```

### When to Use Each

| Type | Use When |
|------|----------|
| `#macro` (global) | Default choice. Use when you need to create/modify variables in calling scope, or define functions. |
| `#macro local` | Safer, no side effects on calling scope. Use for self-contained operations. |

## Argument Syntax

Macro arguments use explicit markers for replacement:

| Syntax | Description | Example (`name` = `count`) |
|--------|-------------|----------------------------|
| `name` | Not replaced (literal) | `name` stays `name` |
| `.name` | Value replacement | `.name` becomes `count` |
| `..name` | Stringification | `..name` becomes `"count"` |

**Note:** Macro argument names cannot start with `.`.

### Basic Example

```r
#macro
LOG_EVAL(expr){
  cat("Evaluating stuff\n")
  .expr
}
#endmacro

LOG_EVAL(sum(1, 1, 1))
```

**Expands to:**

```r
cat("Evaluating stuff\n")
sum(1, 1, 1)
```

### Stringification

Use `..arg` to get the argument as a string literal:

```r
#macro
DEBUG(var){
  cat(..var, "=", .var, "\n")
}
#endmacro

my_value <- 42
DEBUG(my_value)
```

**Expands to:**

```r
my_value <- 42
cat("my_value", "=", my_value, "\n")
```

### Building Identifiers (Token Pasting)

Use `.arg` within identifier names to build new identifiers:

```r
#macro
GETTER(name){
  get_.name <- function() private$.name
}
#endmacro

GETTER(count)
```

**Expands to:**

```r
get_count <- function() private$count
```

## Examples

### Cleanup

Ensure you close your database connections when you're done.

```r
#macro
CONNECT(){
  con <- DBI::dbConnect(RSQLite::SQLite(), ":memory:")
  on.exit(DBI::dbDisconnect(con))
}
#endmacro

CONNECT()
```

**Expands to:**

```r
con <- DBI::dbConnect(RSQLite::SQLite(), ":memory:")
on.exit(DBI::dbDisconnect(con))
```

### Repeat

Macro to easily repeat a block of code.

```r
#macro
REPEAT(n, action){
  for (i in 1:.n) {
    .action
  }
}
#endmacro

REPEAT(3, print("Hello"))
```

**Expands to:**

```r
for (i in 1:3) {
  print("Hello")
}
```

### Macros with Multiple Arguments

Macros can accept multiple arguments, which are separated by commas in both the definition and invocation.

```r
#macro
VALIDATE(value, min, max){
  if (.value < .min || .value > .max) {
    stop("Value out of range: ", .value)
  }
}
#endmacro

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

### Logging Macro

```r
#macro
LOG(level, msg){
  cat("[", .level, "] ", .msg, "\n", sep = "")
}
#endmacro

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
#macro
TRY_CATCH(code, error_msg){
  tryCatch({
    .code
  }, error = function(e) {
    cat(.error_msg, ":", e$message, "\n")
  })
}
#endmacro

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
#macro
TIME_IT(expr){
  start <- Sys.time()
  result <- .expr
  end <- Sys.time()
  cat("Execution time:", end - start, "\n")
  result
}
#endmacro

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

- Macros start with `#macro` (or `#macro local`) alone on a line
- The macro signature and body follow on subsequent lines
- Macro bodies must be enclosed in curly braces `{}`
- Macros end with `#endmacro`
- Global macros (default) expand without wrapping
- Local macros (`#macro local`) are wrapped in `local({...})` to avoid namespace pollution
- Macros support multiline definitions and can span many lines (up to 1024 lines)
- Macro argument names **cannot** start with `.`
- Use `.arg` to substitute the argument value, `..arg` for stringification
- Bare argument names (without `.` prefix) are **not** replaced
- Use `#define NAME value` for simple constants (not function-like macros)
