/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BR_IO_H
#define BR_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
# ifndef F_OK
#  define F_OK 00
# endif
# ifndef W_OK
#  define W_OK 02
# endif
# ifndef R_OK
#  define R_OK 04
# endif
/*
 * Microsoft documentation does not specify mode 01 (X_OK), we better
 * not use this
 */
# ifdef X_OK
#  undef X_OK
# endif
# define X_OK 00

# ifndef S_ISDIR
#  define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
# endif
# ifndef S_ISREG
#  define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
# endif

#endif

ssize_t robread(int fd, void *buf, size_t n);

ssize_t robwrite(int fd, const void *buf, size_t n);

char *readfile(const char *name);

/**
 * mk_file_dir - make file directory
 * @name: file path
 * @note: this function success if the directory already exists
 */
int mk_file_dir(const char *name);

int is_abs_path(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* BR_IO_H */
