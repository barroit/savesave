// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"

const char *exec_path(void)
{
	static const char *path;

	if (!path) {
		char *target;
		int err = readlink_nt("/proc/self/exe", &target);

		BUG_ON(err);
		path = target;
	}

	return path;
}
