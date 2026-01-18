# Constants

Builder provides a special syntax for creating constant (immutable) bindings using the `-<` operator. Once assigned, constant values cannot be changed, providing safety against accidental modification.

## Basic Syntax

Use `-<` instead of `<-` to create a constant binding.

```r
PI -< 3.14159
MAX_SIZE -< 100
```

**Expands to:**

```r
PI <- 3.14159;lockBinding("PI", environment());
MAX_SIZE <- 100;lockBinding("MAX_SIZE", environment());
```

## How It Works

The `-<` operator is transformed during preprocessing into a regular assignment followed by a `lockBinding()` call. This prevents the variable from being reassigned later in the code.

## Usage Examples

### Defining Configuration Constants

```r
API_URL -< "https://api.example.com"
TIMEOUT -< 30
DEBUG -< TRUE

# Later attempts to modify will fail
# API_URL <- "different-url"  # Error: cannot change value of locked binding
```

### Mathematical Constants

```r
EULER -< 2.71828
GOLDEN_RATIO -< 1.61803

calculate_growth <- function(t) {
  EULER ^ t  # Safe to use, cannot be accidentally changed
}
```

### Application Settings

```r
APP_NAME -< "MyApp"
VERSION -< "1.0.0"
MAX_RETRIES -< 3

print(f'{APP_NAME} v{VERSION}')
```

## Benefits

- **Safety:** Prevents accidental modification of critical values
- **Clarity:** Makes it clear which values are meant to be immutable
- **Protection:** Using `lockBinding()` provides R-level protection against reassignment
- **Convention:** Provides a clean syntax for constant declarations

## Important Notes

- Once a binding is locked, attempting to reassign it will result in an error
- The `-<` syntax can be used with any R value: numbers, strings, vectors, lists, functions, etc.
- Whitespace around the `-<` operator is automatically handled during preprocessing
- This is a compile-time transformation, so the locking happens when your code runs
