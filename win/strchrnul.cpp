// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

char *strchrnul(const char *s, int c)
{
	while(*s && *s != c)
		s++;
	return (char *)s;
}
