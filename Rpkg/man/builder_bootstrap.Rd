\name{builder_bootstrap}
\alias{builder_bootstrap}
\title{Bootstrap the builder sources into the R package}
\usage{
builder_bootstrap(repo_root = NULL)
}
\arguments{
\item{repo_root}{Path to the builder repository root.}
}
\description{
Copies the builder C source and header files from the parent builder
repository into \code{inst/builder_src/} so the R package becomes
self-contained.
}
\value{
Invisibly returns the destination directory.
}
