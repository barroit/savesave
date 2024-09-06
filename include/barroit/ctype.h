/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_CTYPE_H
#define BRT_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

static inline int istab(int c)
{
	return c == '\t';
}

static inline int isnewline(int c)
{
	return c == '\n';
}

#ifdef __cplusplus
}
#endif

#endif /* BRT_CTYPE_H */
