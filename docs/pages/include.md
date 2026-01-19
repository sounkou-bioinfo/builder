# Include

The `#include` directive allows you to read external files at build time and process them using macros. This enables you to embed file contents (like SQL queries, templates, or configuration) directly into your R code as variables.

## Advantages

- **Separation of concerns:** Keep SQL queries, templates, and other content in separate files while still embedding them in your code at build time.
- **Build-time processing:** File contents are read and processed during preprocessing, not at runtime, eliminating file I/O overhead.
- **Macro integration:** Use any macro to process file contents, enabling custom transformations and formatting.
- **Type safety:** File contents are converted to R objects at build time, catching errors early.

## Basic Syntax

The `#include` directive uses a special syntax that specifies a macro to apply, a file path to read, and a variable name for the result.

**Syntax:** `#include:MACRO_NAME file_path variable_name`

The directive consists of three space-separated parts:

- `MACRO_NAME` - The name of a previously defined macro that takes one argument (the file path)
- `file_path` - The path to the file to include
- `variable_name` - The R variable name that will be assigned the processed result

## Basic Example

The most common use case is reading text files like SQL queries. First, define a macro that specifies how to read the file:

```r
#define
READ(path){
  readLines(path)
}
#enddef

#include:READ sql/query.sql my_query
```

**Expands to:**

```r
my_query <- c("SELECT * FROM users")
```

The `READ` macro uses R's `readLines()` function to read the file. The result is captured using R's `dput()` function and assigned to the specified variable.

## How It Works

When the preprocessor encounters an `#include` directive:

1. It parses the macro name, file path, and variable name
2. It looks up the macro definition (e.g., `READ`) from your `#define` statements
3. It constructs an R expression: `MACRO_NAME('file_path') |> dput() |> capture.output()`
4. It executes this R expression to process the file
5. It generates an assignment: `variable_name <- <result>`

This happens at build time, so the final R code contains no file reading operations—just the processed data.

### Reading CSV Data

Embed small lookup tables or static data directly in your code:

```r
#define
READ_CSV(path){
  read.csv(path)
}
#enddef

#include:READ_CSV data/lookup_table.csv lookup_data
```

### Reading Template Files

Read template strings for reports or emails:

```r
#define
READ_TEMPLATE(path){
  readLines(path) |> paste(collapse = "\n")
}
#enddef

#include:READ_TEMPLATE templates/email_body.txt email_template
```

## Important Notes

- The macro specified in `#include:MACRO_NAME` must be defined before the `#include` directive
- The macro must accept exactly one parameter (the file path)
- File paths are relative to the build directory or can be absolute paths
- The result is captured using R's `dput()` function, which converts R objects to their code representation
- File reading happens at build time, not runtime, so files must exist when running the builder
- Changes to included files require rebuilding to be reflected in the output
- The macro can use any R code for processing—you're not limited to simple `readLines()`
- Variable names should follow R naming conventions and will be created in the global scope

## Processing Pipeline

The `#include` directive is processed after `#define` macro expansion but as part of the same preprocessing pass. This means:

1. First, all `#define` macros are defined and expanded
2. Then, `#include` directives are processed using the defined macros
3. The result is pure R code with all preprocessing directives removed

## Common Use Cases

- **SQL Queries:** Keep SQL in `.sql` files with proper syntax highlighting and embed them in R packages
- **Configuration:** Store configuration in JSON, YAML, or other formats and embed at build time
- **Static Data:** Include small reference datasets or lookup tables directly in code
- **Templates:** Store HTML, Markdown, or text templates externally and include them for string generation
- **Constants:** Define large constant structures in separate files for better organization
