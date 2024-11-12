// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

char *strchrnul(const char *s, int c)
{
	while(*s && *s != c)
		s++;
	return (char *)s;
}
