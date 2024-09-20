/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PATH_H
#define PATH_H

int is_absolute_path(const char *path);

static inline int is_dir_indicator(const char *name)
{
	return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

const char *get_home_dirname(void);

const char *get_executable_dirname(void);

const char *get_tmp_dirname(void);

#endif /* PATH_H */
