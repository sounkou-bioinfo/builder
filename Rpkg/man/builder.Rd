\name{builder}
\alias{builder}
\title{Run the builder preprocessor}
\usage{
builder(args = character(), ...)
}
\arguments{
\item{args}{Character vector of command-line arguments.}
\item{...}{Additional arguments passed to \code{\link[base]{system2}}.}
}
\description{
Low-level wrapper that calls the \code{builder} binary with arbitrary
arguments via \code{\link[base]{system2}}.
}
\value{
The return value of \code{system2} (by default the exit code).
}
\examples{
\dontrun{
builder(args = "-help")
}
}
