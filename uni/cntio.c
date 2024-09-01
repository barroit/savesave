// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

void count_add1(void);

#ifdef opendir
# undef opendir
#endif

#ifdef closedir
# undef closedir
#endif

DIR *cntopendir(const char *name)
{
	DIR *dir = opendir(name);
	if (dir == NULL)
		return NULL;

	count_add1();
	return dir;
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
