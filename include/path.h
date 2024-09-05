/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PATH_H
#define PATH_H

#ifdef __cplusplus
extern "C" {
#endif

int is_abs_path(const char *path);

static inline int is_dir_indicator(const char *name)
{
	return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

const char *get_home_dir(void);

#ifdef __cplusplus
}
#endif

#endif /* PATH_H */
