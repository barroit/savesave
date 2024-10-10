/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef COPY_H
#define COPY_H

int PLATSPECOF(fd2fd_copy)(int src, int dest, off_t len);
int copy_regfile(const char *srcname, const char *destname);

int copy_symlink(const char *srcname, const char *destname);

#endif /* COPY_H */
