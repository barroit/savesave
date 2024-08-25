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

int robopen2(const char *file, int oflag);
int robopen3(const char *file, int oflag, mode_t mode);
#define robopen(...) FLEXCALL_FUNCTION3(robopen, __VA_ARGS__)

int robclose(int fd);

char *readfile(const char *name);

int is_abs_path(const char *path);

const char *get_home_dir(void);

int calc_dir_size(const char *dir, off_t *size);

#ifdef __cplusplus
}
#endif

#endif /* ROBIO_H */
