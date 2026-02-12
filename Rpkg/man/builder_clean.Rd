\name{builder_clean}
\alias{builder_clean}
\title{Clean builder output directory}
\usage{
builder_clean(output = NULL, ...)
}
\arguments{
\item{output}{Output directory to clean (default \code{"R"}).}
\item{...}{Additional arguments passed to \code{\link{builder}}.}
}
\description{
Runs builder which cleans the output directory.
}
\value{
Exit code from builder.
}
