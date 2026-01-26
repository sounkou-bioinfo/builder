---
title: Architecture
---

# Architecture

Builder assumes you're building a package
and simulates R's lazy loading behavior by performing a two-pass build.
Understanding this architecture helps you write more predictable preprocessing directives.

## Two-Pass System

<div class="desktop-only">

```
┌─────────────────────────────────────────────────────────────┐
│                       FIRST PASS                            │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │  Collect    │  │    Run      │  │  Plugin             │  │
│  │  #define    │──│  #preflight │──│  preprocess hook    │  │
│  │  macros     │  │  blocks     │  │                     │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      SECOND PASS                            │
│  For each line:                                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌────────────┐   │
│  │ F-string │─▶│ #include │─▶│  Macro   │─▶│Deconstruct │   │
│  │ replace  │  │ replace  │  │ replace  │  │  replace   │   │
│  └──────────┘  └──────────┘  └──────────┘  └────────────┘   │
│                                                   │         │
│       ┌───────────────────────────────────────────┘         │
│       ▼                                                     │
│  ┌──────────┐  ┌──────────┐  ┌────────────┐  ┌───────────┐  │
│  │  Const   │─▶│  #test   │─▶│ Directive  │─▶│Conditional│  │
│  │ replace  │  │ collect  │  │   filter   │  │   check   │  │
│  └──────────┘  └──────────┘  └────────────┘  └───────────┘  │
│                                                   │         │
│                                                   ▼         │
│                              Plugin postprocess hook        │
└─────────────────────────────────────────────────────────────┘
```

</div>

## First Pass

The first pass scans all source files to collect definitions and run preflight checks. No output is written during this pass.

### What happens:

1. **Macro Collection** - All `#define` directives are parsed and stored
2. **Preflight Execution** - `#preflight` / `#endflight` blocks are evaluated
3. **Import Processing** - `#import` directives are noted (for namespace prefixing)
4. **Plugin Hook** - The `preprocess` plugin hook is called on each file's content

## Second Pass

The second pass processes each line through a series of replacements and writes the final output.

### Replacement Order

Each line is processed through these transformations in order:

| Order | Transformation | Example |
|-------|---------------|---------|
| 1 | F-strings | `f'{x}'` → `sprintf('%s', x)` |
| 2 | #include: | `#include:READ file.sql q` → `q <- c(...)` |
| 3 | Macro expansion | `MY_MACRO(x)` → expanded code |
| 4 | Deconstruction | `.[a, b] <- fn()` → indexed assignments |
| 5 | Constants | `x -< 1` → `x <- 1;lockBinding(...)` |

After replacements, the line goes through:

- **Conditional compilation** - `#ifdef`, `#ifndef`, `#if`, `#elif`, `#else`, `#endif`
- **Test collection** - `#test` blocks are extracted
- **Error checking** - `#error` directives halt compilation

## Why Order Matters

The replacement order has important implications:

### F-strings First

F-strings are processed before macros, so you can use macro-defined values inside f-strings:

```r
#define VERSION 2.0.0

print(f'Version: {VERSION}')  # VERSION expanded after f-string processing
```

### Constants Last

The constant operator `-<` is processed after all other transformations, ensuring the final value is locked:

```r
#define DEFAULT 42
x -< DEFAULT  # First: DEFAULT → 42, Then: x <- 42;lockBinding("x", environment())
```

## Built-in Definitions

These definitions are automatically available and updated during processing:

| Definition | Description | Updated |
|------------|-------------|---------|
| `__FILE__` | Current source file path | Per file |
| `__LINE__` | Current line number | Per line |
| `__OS__` | Operating system name | Once at start |
| `__DATE__` | Build date (YYYY-MM-DD) | Once at start |
| `__TIME__` | Build time (HH:MM:SS) | Once at start |
| `__COUNTER__` | Auto-incrementing integer | Per occurrence |
