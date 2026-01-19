# Prepend and append

With the `-prepend` and `-append` arguments, you can specify a file to prepend or append to every output file.

## Example

```bash
builder -input srcr/ -output R/ -prepend LICENSE
```

The license.txt file will be prepended to every output file, and the append.txt file will be appended to every output file.
