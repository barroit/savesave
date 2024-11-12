/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef BRT_CTYPE_H
#define BRT_CTYPE_H

static inline int istab(int c)
{
	return c == '\t';
}

static inline int isln(int c)
{
	return c == '\n';
}

static inline int issep(int ch)
{
#ifdef _WIN32
	return ch == '\\' || ch == '/';
#else
	return ch == '/';
#endif
}

#endif /* BRT_CTYPE_H */
