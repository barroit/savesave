// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mkdir.h"
#include "path.h"

int mkdirp2(char *name, size_t start)
{
	int err;
	char *next = &name[start];

	if (is_absolute_path(name)) {
		char *p = strchr(&name[start], '/');
		if (p)
			next = p + 1;
	}

	while (39) {
		next = strchr(next, '/');
		if (next)
			*next = 0;

		err = MKDIR(name);
		if (err && errno != EEXIST)
			return err;

		if (!next)
			break;

		*next = '/';
		next += 1;
	}

	return 0;
}
