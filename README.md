# builder

A simple preprocessor for R: use macros, directives, and conditionals to build generate your code.

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

#define LOGGER(msg){
# cat("[INFO]", msg, "\n")
#}

#define PI 3.14

PI + 1L

foo <- function(){
    LOGGER("Running foo")

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
}
```

Then build with `./builder -input srcr -DDEBUG -DTEST '"hello world"' -DLOG_LEVEL 42` 
to include the `DEBUG`, and `TEST` directives in the `output` directory.

Currently supports `#if`, `#ifdef`, `#ifndef`, `#else`, `#endif`, `#define` as well as macros.
