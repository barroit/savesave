// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BR_IO_H
#define BR_IO_H

ssize_t robread(int fd, void *buf, size_t n);
ssize_t robwrite(int fd, const void *buf, size_t n);

#endif /* BR_IO_H */
