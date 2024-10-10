// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"

const char *get_executable_dirname(void)
{
	static const char *path;

	if (!path) {
		char *target;
		int err = readlink_notrunc("/proc/self/exe", &target);

		BUG_ON(err);
		path = target;
	}

	return path;
}
