/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_IO_H
#define BRT_IO_H

#ifdef __cplusplus
extern "C" {
#endif

ssize_t robread(int fd, void *buf, size_t n);

ssize_t robwrite(int fd, const void *buf, size_t n);

char *readfile(const char *name);

int is_abs_path(const char *path);

const char *get_home_dir(void);

/**
 * calc_dir_size - calculate total file size in directory
 */
int calc_dir_size(const char *dir, off_t *size);

#ifdef __cplusplus
}
#endif

#endif /* BRT_IO_H */
