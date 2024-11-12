/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef COPY_H
#define COPY_H

int FEATSPEC(fd2fd_copy)(int src, int dest, off_t len);
int copy_regfile(const char *srcname, const char *destname);

int copy_symlink(const char *srcname, const char *destname);

/*
 * An extended function for fileiter
 * NB: The first element of data must be a pointer to struct strbuf, which
 * contains the prefix of destination.
 */
struct iterfile;
int __fiter_cpy_nonreg(struct iterfile *src, void *data);

#ifdef FILEITER_H
# define __FITER_CPY_NONREG \
	(FITER_LIST_DIR | (FITER_LIST_DIR_ONLY & ~FITER_NO_SYMLINK))
#endif

#endif /* COPY_H */
