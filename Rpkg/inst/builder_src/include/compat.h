#ifndef COMPAT_H
#define COMPAT_H

/*
 * Portability shims for Windows / MinGW (Rtools).
 * Include this header in any .c file that uses POSIX-only APIs.
 */

#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <direct.h>   /* _mkdir */
#include <io.h>

/* mkdir: Windows takes 1 arg, POSIX takes 2 */
#define builder_mkdir(path, mode) _mkdir(path)

/* setenv: use _putenv_s on Windows */
static inline int builder_setenv(const char *name, const char *value, int overwrite) {
  (void)overwrite;
  return _putenv_s(name, value);
}

/* popen / pclose: prefixed with underscore on Windows */
#define builder_popen  _popen
#define builder_pclose _pclose

/* asprintf: not available on MinGW, provide a simple implementation */
static inline int builder_asprintf(char **strp, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  if (len < 0) return -1;
  *strp = (char *)malloc((size_t)len + 1);
  if (!*strp) return -1;
  va_start(ap, fmt);
  vsnprintf(*strp, (size_t)len + 1, fmt, ap);
  va_end(ap);
  return len;
}
#define asprintf builder_asprintf

/* d_type / DT_DIR: not reliable on MinGW, use stat() fallback */
#include <sys/stat.h>
static inline int builder_is_dir(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) return 0;
  return S_ISDIR(st.st_mode);
}

/* uname: not available on Windows */
#define BUILDER_NO_UTSNAME 1

#else /* POSIX */

#include <unistd.h>
#include <sys/stat.h>

#define builder_mkdir(path, mode) mkdir(path, mode)
#define builder_setenv(name, value, overwrite) setenv(name, value, overwrite)
#define builder_popen  popen
#define builder_pclose pclose

static inline int builder_is_dir(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) return 0;
  return S_ISDIR(st.st_mode);
}

#endif /* _WIN32 */

#endif /* COMPAT_H */
