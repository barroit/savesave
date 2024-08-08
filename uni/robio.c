// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "termsg.h"

int mk_file_dir(const char *name)
{
	int err;

	char *cp = xstrdup(name);
	const char *dir = dirname(cp);

	struct stat st;
	err = stat(dir, &st);
	if (!err) {
		if (!S_ISDIR(st.st_mode)) {
			warn("file ‘%s’ is not a directory", dir);
			errno = EEXIST;
			err = 1;
		}
	} else if (errno == ENOENT) {
		err = mkdir(dir, 0775);
	}

	free(cp);
	return !!err;
}
