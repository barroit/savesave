/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef MKDIR_H
#define MKDIR_H

#ifdef __cplusplus
extern "C" {
#endif

int mkdirp2(char *name, size_t start);

static inline int mkdirp(char *name)
{
	return mkdirp2(name, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* MKDIR_H */
