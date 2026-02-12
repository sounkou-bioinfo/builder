\name{builder_create}
\alias{builder_create}
\title{Create a new package skeleton}
\usage{
builder_create(name, ...)
}
\arguments{
\item{name}{Name of the package to create.}
\item{...}{Additional arguments passed to \code{\link{builder}}.}
}
\description{
Runs \code{builder -create <name>} to scaffold a new R package
that uses builder.
}
\value{
Exit code from builder.
}
