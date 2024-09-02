// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

void cntio_cntadd1(void);
void cntio_cntsub1(void);

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

	cntio_cntadd1();
	return dir;
}

int cntclosedir(DIR *dirp)
{
	int err;

	err = closedir(dirp);
	if (err)
		return -1;

	cntio_cntsub1();
	return 0;
}
