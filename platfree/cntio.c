// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifdef CONFIG_CC_IS_CLANG
# pragma GCC diagnostic ignored "-Wsometimes-uninitialized"
#endif

#include "debug.h"
#include "robio.h"

static atomic_uint fd_count;

void cntio_cntadd1(void)
{
	atomic_fetch_add(&fd_count, 1);
}

void cntio_cntsub1(void)
{
	atomic_fetch_sub(&fd_count, 1);
	BUG_ON(atomic_load(&fd_count) == UINT_MAX);
}

int cntcreat(const char *file, mode_t mode)
{
	int fd = robcreat(file, mode);
	if (fd == -1)
		return -1;

	cntio_cntadd1();
	return fd;
}

int cntopen2(const char *file, int oflag)
{
	return cntopen3(file, oflag, 0);
}

int cntopen3(const char *file, int oflag, mode_t mode)
{
	int fd = robopen(file, oflag, mode);
	if (fd == -1)
		return -1;

	cntio_cntadd1();
	return fd;
}

int cntclose(int fd)
{
	int err;

	err = robclose(fd);
	if (err)
		return -1;

	cntio_cntsub1();
	return 0;
}

int cntdup2(int oldfd, int newfd)
{
	int fd = robdup2(oldfd, newfd);
	if (fd != -1)
		cntio_cntadd1();

	return fd;
}
