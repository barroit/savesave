// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifdef opendir
# undef opendir
#endif

DIR *cntopendir(const char *name)
{
	DIR *dir = opendir(name);

	if (likely(dir != NULL))
		__atomic_fetch_add(&cntio_fdcnt, 1, __ATOMIC_RELAXED);

	return dir;
}

#ifdef closedir
# undef closedir
#endif

int cntclosedir(DIR *dirp)
{
	int ret = closedir(dirp);

	if (likely(ret == 0)) {
		uint prev = __atomic_fetch_sub(&cntio_fdcnt,
					       1, __ATOMIC_RELAXED);
		BUG_ON(prev == 0);
	}

	return ret;
}
