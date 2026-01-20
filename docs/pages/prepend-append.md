# Prepend and Append

With the `-prepend` and `-append` flags, you can specify files to prepend or append to every output file.

## Usage

```bash
builder -input srcr -output R -prepend header.txt -append footer.txt
```

- `-prepend <file>` - Content added to the start of every output file
- `-append <file>` - Content added to the end of every output file

## Example: License Header

Add a license header to all generated R files:

```bash
builder -input srcr -output R -prepend LICENSE
```

The contents of `LICENSE` will be prepended to every output file in `R/`.
