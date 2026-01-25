---
title: Plugins
---

# Plugins

Builder supports plugins to extend processing.
Plugins are R packages that export a function returning a list of lifecycle methods.

See [builder.air](https://github.com/devOpifex/builder.air) for a simple example plugin.

If your plugin requires configuration use a config file.

## Usage

Specify plugins with the `-plugin` flag:

```bash
./builder -input srcr -plugin package::function
```

The format is `package::function` where `package` is the R package name and `function` is the exported function that creates the plugin.

## Lifecycle Methods

Plugins can implement the following methods:

### setup(input, output)

Called during initialization with the input and output directory paths. Use this to store configuration or perform one-time setup.

### preprocess(str, file)

Called on each file's content before Builder processes it. 
Receives the file content as a string and should return the modified content.

### postprocess(str, file)

Called on each file's content after Builder processes it. Receives the file content as a string and should return the modified content.

### include(type, path, object, file)

Called for each `#include` directive with parsed components:

- `type` - The file type (e.g., `"csv"`, `"json"`)
- `path` - The file path to include
- `object` - The variable name for the result
- `file` - The source file being processed

Return `NULL` to use default processing, or return a replacement line.

### end()

Called when Builder finishes processing all files. Use this for cleanup or final operations.

## Example Plugin

A simple minifier plugin that replaces newlines with semicolons (it's a toy example that will break your code):

```r
#' @export
plugin <- function() {
  list(
    setup = function(input, output, ...) {},
    preprocess = function(str, file, ...) {
      lines <- strsplit(str, "\n")[[1]]
      lines <- lines[nzchar(trimws(lines))]
      paste(lines, collapse = ";")
    },
    postprocess = function(str, file, ...) {},
    end = function(...) {},
    include = function(type, path, object, file, ...) {
      # Return NULL to use default processing
      NULL
    }
  )
}
```

Use it with `-plugin minifier::plugin`. Methods that don't need to modify content can return `NULL` or be empty. Use `...` in function signatures for forward compatibility.
