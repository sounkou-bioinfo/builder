\name{builder_build}
\alias{builder_build}
\title{Build (preprocess) R sources}
\usage{
builder_build(
  input = NULL,
  output = NULL,
  defines = NULL,
  imports = NULL,
  plugins = NULL,
  depends = NULL,
  deadcode = FALSE,
  sourcemap = FALSE,
  prepend = NULL,
  append = NULL,
  noclean = FALSE,
  ...
)
}
\arguments{
\item{input}{Input directory (default \code{"srcr"}).}
\item{output}{Output directory (default \code{"R"}).}
\item{defines}{Named character vector of macro definitions, e.g.
\code{c(DEBUG = "", VALUE = "42")}.}
\item{imports}{Character vector of import paths.}
\item{plugins}{Character vector of plugin specifications (\code{"pkg::fn"}).}
\item{depends}{Character vector of package dependencies.}
\item{deadcode}{Logical; enable dead code detection?}
\item{sourcemap}{Logical; enable source map generation?}
\item{prepend}{Path to file whose contents are prepended to every output file.}
\item{append}{Path to file whose contents are appended to every output file.}
\item{noclean}{Logical; skip cleaning output directory before build?}
\item{...}{Additional arguments passed to \code{\link{builder}}.}
}
\description{
Runs the builder preprocessor on the given input directory, writing
processed files to the output directory.
}
\value{
Exit code from builder (0 on success).
}
\examples{
\dontrun{
builder_build(input = "srcr", output = "R", defines = c(DEBUG = ""))
}
}
