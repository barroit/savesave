// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

void strrepl(char *str, int c, int r)
{
	while ((str = strchr(str, c)) != NULL)
		*str++ = r;
}

char *straftr(const char *str, const char *pref)
{
	do {
		if (!*pref)
			return (char *)str;
	} while (*str++ == *pref++);

	return NULL;
}

