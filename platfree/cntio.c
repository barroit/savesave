// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifdef CONFIG_CC_IS_CLANG
# pragma GCC diagnostic ignored "-Wsometimes-uninitialized"
#endif

#include "robio.h"

rlim_t cntio_fdcnt;

int cntcreat(const char *file, mode_t mode)
{
	int fd = robcreat(file, mode);

	if (likely(fd != -1))
		__atomic_fetch_add(&cntio_fdcnt, 1, __ATOMIC_RELAXED);

	return fd;
}

int cntopen2(const char *file, int oflag)
{
	return cntopen3(file, oflag, 0);
}

int cntopen3(const char *file, int oflag, mode_t mode)
{
	int fd = robopen(file, oflag, mode);

	if (likely(fd != -1))
		__atomic_fetch_add(&cntio_fdcnt, 1, __ATOMIC_RELAXED);

	return fd;
}

int cntclose(int fd)
{
	BUG_ON(fd == -1);

	int ret = robclose(fd);

	if (likely(ret == 0)) {
		uint prev = __atomic_fetch_sub(&cntio_fdcnt,
					       1, __ATOMIC_RELAXED);
		BUG_ON(prev == 0);
	}

	return ret;
}

int cntdup(int oldfd)
{
	int fd = robdup(oldfd);

	if (likely(fd != -1))
		__atomic_fetch_add(&cntio_fdcnt, 1, __ATOMIC_RELAXED);

	return fd;
}

int cntdup2(int oldfd, int newfd)
{
	int fd = robdup2(oldfd, newfd);

	if (likely(fd != -1))
		__atomic_fetch_add(&cntio_fdcnt, 1, __ATOMIC_RELAXED);

	return fd;
}
