# Packages

Builder reads your R files from the srcr (default) directory and writes the output to the R (default) directory.

```bash
./builder -input srcr -output R
```

This allows building R packages from **multiple**, **nested** directories such as:

```bash
srcr
├── main.R
└── sub
    └── main.R
```

That will produce:

```bash
R
├── main.R
└── sub-main.R
```

Note that the file order is preserved, though it's unlikely to matter in an R package.
