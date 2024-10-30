// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "robio.h"
#include "barrier.h"

rlim_t cntio_fdcnt;

int cntcreat(const char *file, mode_t mode)
{
	int fd = robcreat(file, mode);

	if (likely(fd != -1))
		smp_inc_return(&cntio_fdcnt);

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
		smp_inc_return(&cntio_fdcnt);

	return fd;
}

int cntclose(int fd)
{
	BUG_ON(fd == -1);

	int ret = robclose(fd);

	if (likely(ret == 0)) {
		uint new = smp_dec_return(&cntio_fdcnt);
		BUG_ON(new == -1);
	}

	return ret;
}

int cntdup(int oldfd)
{
	int fd = robdup(oldfd);

	if (likely(fd != -1))
		smp_inc_return(&cntio_fdcnt);

	return fd;
}

int cntdup2(int oldfd, int newfd)
{
	int fd = robdup2(oldfd, newfd);

	if (likely(fd != -1))
		smp_inc_return(&cntio_fdcnt);

	return fd;
}
