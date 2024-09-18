/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef MKDIR_H
#define MKDIR_H

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

int flexremove(const char *name);

struct fileiter_file;
int PLATSPECOF(sizeof_file)(struct fileiter_file *file, void *data);

/**
 * calc_dir_size - calc. directory size
 *
 * note: this function handles error
 */
int calc_dir_size(const char *name, off_t *size);

#endif /* MKDIR_H */
