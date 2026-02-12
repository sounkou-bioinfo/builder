# ---------------------------------------------------------------------------
# Bootstrap: copy builder C sources into inst/builder_src/
# ---------------------------------------------------------------------------

#' Bootstrap the builder sources into the R package
#'
#' Copies the builder C source and header files from the parent builder
#' repository into \code{inst/builder_src/} so the R package becomes
#' self-contained. Run this before \code{R CMD build} to create the
#' source tarball.
#'
#' @param repo_root Path to the builder repository root. Default auto-detects
#'   from the package source directory (assumes \code{<repo>/Rpkg}).
#' @return Invisibly returns the destination directory.
#' @export
builder_bootstrap <- function(repo_root = NULL) {
    if (is.null(repo_root)) {
        repo_root <- builder_find_repo()
    }

    repo_root <- normalizePath(repo_root, mustWork = TRUE)
    message("Repo root: ", repo_root)

    if (!file.exists(file.path(repo_root, "src", "main.c"))) {
        stop("Not a builder repo: missing src/main.c", call. = FALSE)
    }

    pkg_dir <- file.path(repo_root, "Rpkg")
    dest <- file.path(pkg_dir, "inst", "builder_src")

    if (dir.exists(dest)) {
        unlink(dest, recursive = TRUE)
    }
    dir.create(dest, recursive = TRUE, showWarnings = FALSE)
    message("Destination: ", dest)

    # C source files
    src_dir <- file.path(repo_root, "src")
    c_files <- list.files(src_dir, pattern = "\\.c$", full.names = FALSE)
    file.copy(file.path(src_dir, c_files), dest)
    message("  Copied ", length(c_files), " C source files")

    # Header files
    inc_dir <- file.path(repo_root, "include")
    inc_dest <- file.path(dest, "include")
    dir.create(inc_dest, showWarnings = FALSE)
    h_files <- list.files(inc_dir, pattern = "\\.h$", full.names = FALSE)
    file.copy(file.path(inc_dir, h_files), inc_dest)
    message("  Copied ", length(h_files), " header files")

    message("Bootstrap complete. Run 'R CMD build .' to create the tarball.")
    invisible(dest)
}


# Try to find the builder repo root
builder_find_repo <- function() {
    # Assume Rpkg/ is inside the builder repo
    pkg_dir <- getwd()
    candidate <- dirname(pkg_dir)

    if (file.exists(file.path(candidate, "src", "main.c"))) {
        return(candidate)
    }

    # Try going up one more level
    candidate2 <- dirname(candidate)
    if (file.exists(file.path(candidate2, "src", "main.c"))) {
        return(candidate2)
    }

    stop(
        "Cannot auto-detect builder repo root. ",
        "Pass repo_root explicitly or run from Rpkg/.",
        call. = FALSE
    )
}
