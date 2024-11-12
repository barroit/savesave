// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "barrier.h"

#ifdef opendir
# undef opendir
#endif

DIR *cntopendir(const char *name)
{
	DIR *dir = opendir(name);

	if (likely(dir != NULL))
		smp_inc_return(&cntio_fdcnt);

	return dir;
}

#ifdef closedir
# undef closedir
#endif

int cntclosedir(DIR *dirp)
{
	int ret = closedir(dirp);

	if (likely(ret == 0)) {
		uint new = smp_dec_return(&cntio_fdcnt);
		BUG_ON(new == -1);
	}

	return ret;
}

#ifdef pipe
# undef pipe
#endif

int cntpipe(int pipedes[2])
{
	int ret = pipe(pipedes);

	if (likely(ret == 0))
		smp_add_return(&cntio_fdcnt, 2);

	return ret;
}
