---
title: Watch Mode
---

# Watch Mode

Builder can automatically rebuild your package when source files change using the `-watch` flag. This is useful during active development to see changes immediately without manually running the build command.

## Usage

```bash
builder -watch
builder -watch -input srcr -output R
```

## How It Works

When watch mode is enabled:

1. Builder performs an initial build (with cleaning unless `-noclean` is specified)
2. Sets up file system monitoring on the input directory
3. Waits for file changes (create, modify, delete)
4. Automatically rebuilds when changes are detected
5. Repeats until interrupted with Ctrl+C

## Example

```bash
$ builder -watch
[INFO] Watch mode enabled, monitoring srcr
[INFO] Cleaning: R/ and testthat/
[INFO] Copying srcr/main.R to R/main.R
[INFO] Waiting for changes...

# Edit srcr/main.R in your editor...

[INFO] Change detected, rebuilding...
[INFO] Copying srcr/main.R to R/main.R
[INFO] Waiting for changes...

# Press Ctrl+C to exit
```

## Options

| Flag | Description |
|------|-------------|
| `-watch` | Enable watch mode |
| `-noclean` | Skip cleaning on initial build |

## Behavior

- **Initial build**: Cleans output directory (unless `-noclean`)
- **Subsequent rebuilds**: Skip cleaning to preserve any manual changes
- **Error handling**: Build errors are reported but don't stop watching
- **Exit**: Press Ctrl+C to stop watching

## Technical Details

Watch mode uses Linux's `inotify` API to efficiently monitor file changes. It watches for:

- File saves (close after write)
- File renames/moves (vim-style atomic saves)
- File deletions

A 100ms debounce prevents multiple rebuilds from rapid successive saves.

## Limitations

- Only the input directory (`-input`) is monitored
- Imported header files (`-import`) are not watched; restart watch mode if headers change
- Linux only (uses `inotify`); macOS support would require `kqueue`
