---
title: Plugins
---

# Plugins

Builder supports plugins to extend processing. Plugins are R packages that export a function returning a list of lifecycle methods.

---

## Using Plugins

### Via CLI

Specify plugins with the `-plugin` flag:

```bash
builder -plugin package::function
```

### Via Config File

Add plugins to your `builder.ini`:

```ini
plugin: package::function
```

### Multiple Plugins

You can use multiple plugins. They are called in order.

**CLI:**
```bash
builder -plugin pkg1::fn1 -plugin pkg2::fn2
```

**Config (space-separated):**
```ini
plugin: pkg1::fn1 pkg2::fn2
```

### Format

The format is `package::function` where:

- `package` - The R package name
- `function` - The exported function that creates the plugin

---

## Developing Plugins

### Overview

A plugin is an R function that returns a list of lifecycle methods. Each method is called at a specific point during the build process.

See [builder.air](https://github.com/devOpifex/builder.air) for a real-world example plugin.

### Lifecycle Methods

#### setup(input, output)

Called during initialization with the input and output directory paths. Use this to store configuration or perform one-time setup.

#### preprocess(str, file)

Called on each file's content **before** Builder processes it. Receives the file content as a string and should return the modified content.

#### postprocess(str, file)

Called on each file's content **after** Builder processes it. Receives the file content as a string and should return the modified content.

#### include(type, path, object, file)

Called for each `#include` directive with parsed components:

- `type` - The file type (e.g., `"csv"`, `"json"`)
- `path` - The file path to include
- `object` - The variable name for the result
- `file` - The source file being processed

Return `NULL` to use default processing, or return a replacement line.

#### end()

Called when Builder finishes processing all files. Use this for cleanup or final operations.

### Example Plugin

A simple minifier plugin that removes empty lines and joins with semicolons:

```r
#' @export
plugin <- function() {
 list(

   setup = function(input, output, ...) {
     # Store paths or initialize state
   },
   preprocess = function(str, file, ...) {
     # Modify content before processing
     lines <- strsplit(str, "\n")[[1]]
     lines <- lines[nzchar(trimws(lines))]
     paste(lines, collapse = ";")
   },
   postprocess = function(str, file, ...) {
     # Modify content after processing
     str
   },
   include = function(type, path, object, file, ...) {
     # Handle #include directives
     # Return NULL to use default processing
     NULL
   },
   end = function(...) {
     # Cleanup or final operations
   }
 )
}
```

Use it with `-plugin minifier::plugin`.

### Tips

- Use `...` in function signatures for forward compatibility
- Methods that don't modify content can return `NULL`
- If your plugin requires configuration, read from a config file in `setup()`
- Plugins are called in the order they are specified
