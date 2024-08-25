// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifdef CONFIG_CC_IS_CLANG
# pragma GCC diagnostic ignored "-Wsometimes-uninitialized"
#endif

#include "cntio.h"
#include "debug.h"
#include "robio.h"

#ifdef open
# undef open
#endif

#ifdef opendir
# undef opendir
#endif

#ifdef close
# undef close
#endif

#ifdef closedir
# undef closedir
#endif

atomic_uint fd_count = 0;

static inline void count_add1(void)
{
	atomic_fetch_add(&fd_count, 1);
}

static inline void count_sub1(void)
{
	atomic_fetch_sub(&fd_count, 1);
	BUG_ON(atomic_load(&fd_count) == UINT_MAX);
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

	count_add1();
	return fd;
}

DIR *cntopendir(const char *name)
{
	DIR *dir = opendir(name);
	if (dir == NULL)
		return NULL;

	count_add1();
	return dir;
}

int cntclose(int fd)
{
	int err;

	err = robclose(fd);
	if (err)
		return -1;

	count_sub1();
	return 0;
}

int cntclosedir(DIR *dirp)
{
	int err;

	err = closedir(dirp);
	if (err)
		return -1;

	count_sub1();
	return 0;
}
