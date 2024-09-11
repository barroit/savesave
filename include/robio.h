/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef ROBIO_H
#define ROBIO_H

#ifdef __cplusplus
extern "C" {
#endif

ssize_t robread(int fd, void *buf, size_t n);

ssize_t robwrite(int fd, const void *buf, size_t n);

ssize_t robread_all(int fd, void *buf, size_t n);

ssize_t robwrite_all(int fd, const void *buf, size_t n);

int robcreat(const char *file, mode_t mode);

int robopen2(const char *file, int oflag);
int robopen3(const char *file, int oflag, mode_t mode);
#define robopen(...) FLEXCALL_FUNCTION3(robopen, __VA_ARGS__)

int robclose(int fd);

int robdup2(int oldfd, int newfd);

#ifdef __cplusplus
}
#endif

#endif /* ROBIO_H */
