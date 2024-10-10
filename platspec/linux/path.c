// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"

const char *get_executable_dirname(void)
{
	static const char *path;

	if (!path) {
		char buf[PATH_MAX];

		ssize_t nr = readlink("/proc/self/exe",
				      buf, sizeof_array(buf));
		BUG_ON(nr == -1 || nr == sizeof_array(buf));

		buf[nr] = 0;
		path = dirname(buf);
		path = xstrdup(path);
	}

	return path;
}
