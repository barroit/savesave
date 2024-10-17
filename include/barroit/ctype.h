/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
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

#endif /* BRT_CTYPE_H */
