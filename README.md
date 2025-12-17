# builder

A small CLI tool to build R packages from multiple directories and preprocessor directives.

## Installation

```bash
git clone https://github.com/devOpifex/builder.git
make
```

## Usage

```bash
./builder -input srcr 
```

You can also use the `# ifdef` directives.

```r
x <- 1;
# ifdef DEBUG
cat("x is", x, "\n");
# endif

x <- x + 1
```

Then build with `./builder -input srcr -DDEBUG`.
