# ---------------------------------------------------------------------------
# system2 wrappers around the builder binary
# ---------------------------------------------------------------------------

#' Get the path to the builder binary
#'
#' Returns the full path to the compiled \code{builder} binary shipped
#' with the package.
#'
#' @return A single character string containing the path to the binary.
#' @export
builder_path <- function() {
    bin <- system.file("bin", "builder", package = "builder", mustWork = FALSE)

    if (!nzchar(bin) || !file.exists(bin)) {
        stop(
            "builder binary not found. ",
            "The package may not have been installed correctly.",
            call. = FALSE
        )
    }

    bin
}

#' Run the builder preprocessor
#'
#' Low-level wrapper that calls the \code{builder} binary with arbitrary
#' arguments via \code{\link[base]{system2}}.
#'
#' @param args Character vector of command-line arguments.
#' @param ... Additional arguments passed to \code{\link[base]{system2}}.
#' @return The return value of \code{system2} (by default the exit code).
#' @export
#' @importFrom utils tail
builder <- function(args = character(), ...) {
    system2(builder_path(), args = args, ...)
}

#' Get builder version
#'
#' @return A character string with the version info (printed to stdout).
#' @export
builder_version <- function() {
    out <- builder(args = "-version", stdout = TRUE, stderr = TRUE)
    paste(out, collapse = "\n")
}

#' Build (preprocess) R sources
#'
#' Runs the builder preprocessor on the given input directory, writing
#' processed files to the output directory.
#'
#' @param input Input directory (default \code{"srcr"}).
#' @param output Output directory (default \code{"R"}).
#' @param defines Named character vector of macro definitions, e.g.
#'   \code{c(DEBUG = "", VALUE = "42")}. Names become \code{-DNAME},
#'   non-empty values become the macro value.
#' @param imports Character vector of import paths.
#' @param plugins Character vector of plugin specifications
#'   (\code{"pkg::fn"}).
#' @param depends Character vector of package dependencies.
#' @param deadcode Logical; enable dead code detection?
#' @param sourcemap Logical; enable source map generation?
#' @param prepend Path to file whose contents are prepended to every
#'   output file.
#' @param append Path to file whose contents are appended to every
#'   output file.
#' @param noclean Logical; skip cleaning output directory before build?
#' @param ... Additional arguments passed to \code{\link{builder}}.
#' @return Exit code from builder (0 on success).
#' @export
builder_build <- function(input = NULL,
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
                          ...) {
    args <- character()

    if (!is.null(input)) {
        args <- c(args, "-input", input)
    }

    if (!is.null(output)) {
        args <- c(args, "-output", output)
    }

    if (!is.null(defines)) {
        for (nm in names(defines)) {
            val <- defines[[nm]]
            if (nzchar(val)) {
                args <- c(args, paste0("-D", nm), val)
            } else {
                args <- c(args, paste0("-D", nm))
            }
        }
    }

    if (!is.null(imports)) {
        args <- c(args, "-import", imports)
    }

    if (!is.null(plugins)) {
        args <- c(args, "-plugin", plugins)
    }

    if (!is.null(depends)) {
        args <- c(args, "-depends", depends)
    }

    if (isTRUE(deadcode)) {
        args <- c(args, "-deadcode")
    }

    if (isTRUE(sourcemap)) {
        args <- c(args, "-sourcemap")
    }

    if (!is.null(prepend)) {
        args <- c(args, "-prepend", prepend)
    }

    if (!is.null(append)) {
        args <- c(args, "-append", append)
    }

    if (isTRUE(noclean)) {
        args <- c(args, "-noclean")
    }

    builder(args = args, ...)
}

#' Watch for changes and rebuild
#'
#' Starts builder in watch mode, monitoring the input directory and
#' rebuilding on changes. This is a blocking call.
#'
#' @inheritParams builder_build
#' @param ... Additional arguments passed to \code{\link{builder}}.
#' @return Exit code from builder.
#' @export
builder_watch <- function(input = NULL, output = NULL, ...) {
    args <- "-watch"

    if (!is.null(input)) {
        args <- c(args, "-input", input)
    }

    if (!is.null(output)) {
        args <- c(args, "-output", output)
    }

    builder(args = args, ...)
}

#' Initialise a builder configuration file
#'
#' Creates a \code{builder.ini} configuration file in the current
#' working directory.
#'
#' @param ... Additional arguments passed to \code{\link{builder}}.
#' @return Exit code from builder.
#' @export
builder_init <- function(...) {
    builder(args = "-init", ...)
}

#' Create a new package skeleton
#'
#' Runs \code{builder -create <name>} to scaffold a new R package
#' that uses builder.
#'
#' @param name Name of the package to create.
#' @param ... Additional arguments passed to \code{\link{builder}}.
#' @return Exit code from builder.
#' @export
builder_create <- function(name, ...) {
    builder(args = c("-create", name), ...)
}

#' Clean builder output directory
#'
#' Runs builder with the default settings (which cleans the output
#' directory) but immediately exits.
#'
#' @param output Output directory to clean (default \code{"R"}).
#' @param ... Additional arguments passed to \code{\link{builder}}.
#' @return Exit code from builder.
#' @export
builder_clean <- function(output = NULL, ...) {
    args <- character()
    if (!is.null(output)) {
        args <- c(args, "-output", output)
    }
    builder(args = args, ...)
}
