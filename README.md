# builder

A small CLI tool to build R packages from multiple directories and preprocessor directives.

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
#define PI 3.14

foo <- function(){
    #ifdef DEBUG
    print("debuging")
    #endif

    PI + 1L
}
```

Then build with `./builder -input srcr -DDEBUG` to include the `DEBUG` directive in the `output` directory.

Currently supports `#ifdef`, `#ifndef`, `#else`, `#endif`, `#define` only.
