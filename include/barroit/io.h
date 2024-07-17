// SPDX-License-Identifier: GPL-3.0
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
# include <io.h>
# define USERHOME "USERPROFILE"
#else /* linux */
# include <unistd.h>
# define USERHOME "HOME"
#endif

ssize_t robread(int fd, void *buf, size_t n);

ssize_t robwrite(int fd, const void *buf, size_t n);

char *readfile(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* BR_IO_H */
