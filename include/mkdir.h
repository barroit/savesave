/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef MKDIR_H
#define MKDIR_H

int mkdirp2(char *name, size_t start, int dcheck);

/**
 * mkdirp - make directory as well as parent
 *
 * return: 0 on success, otherwise an error occurred, and errno is set
 */
static inline int mkdirp(char *name)
{
	return mkdirp2(name, 0, 0);
}

int mkfdirp2(char *name, size_t start);

/*
 * Make file directory as well as parent
 */
static inline int mkfdirp(char *name)
{
	return mkfdirp2(name, 0);
}

/*
 * Make file directory as well as parent. Take care of existing files make sure
 * they are directories.
 */
int mkfdirp3(char *name);

/**
 * rmdirr - remove directory as well as subdirectory
 *
 * return: 0 on success, otherwise an error occurred
 */
int rmdirr(const char *name);

/**
 * flexremove - remove file or directory
 */
int flexremove(const char *name);

#endif /* MKDIR_H */
