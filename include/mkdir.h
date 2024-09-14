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

/**
 * mkdirp - make directory as well as parent
 *
 * return: 0 on success, otherwise an error occurred, and errno is set
 */
static inline int mkdirp(char *name)
{
	return mkdirp2(name, 0);
}

/**
 * rmdirr - remove directory as well as subdirectory
 *
 * return: 0 on success, otherwise an error occurred
 */
int rmdirr(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* MKDIR_H */
