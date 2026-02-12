\name{builder_watch}
\alias{builder_watch}
\title{Watch for changes and rebuild}
\usage{
builder_watch(input = NULL, output = NULL, ...)
}
\arguments{
\item{input}{Input directory.}
\item{output}{Output directory.}
\item{...}{Additional arguments passed to \code{\link{builder}}.}
}
\description{
Starts builder in watch mode. This is a blocking call.
}
\value{
Exit code from builder.
}
