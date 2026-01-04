# builder

A small CLI tool to build R packages from multiple directories and use preprocessor directives.

Only works on Linux and macOS.

## Installation

```bash
git clone https://github.com/devOpifex/builder.git
make
```

## Usage

See `builder -help` for usage.

```bash
./builder -input srcr 
```

You can also use the `# ifdef` directives.

```r
# srcr/main.R

#define PI 3.14

foo <- function(){
    #ifdef DEBUG
    print("debuging\n")
    #endif

    #ifdef TEST
    print(TEST)
    #else
    print("no value\n")
    #endif

    #if LOG_LEVEL > 2
    cat("debuging level 2\n")
    #endif

    PI + 1L
}
```

Then build with `./builder -input srcr -DDEBUG -DTEST '"hello world"' -DLOG_LEVEL 42` 
to include the `DEBUG`, and `TEST` directives in the `output` directory.

Currently supports `#if`, `#ifdef`, `#ifndef`, `#else`, `#endif`, `#define` only.
