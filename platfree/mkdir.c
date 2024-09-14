// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mkdir.h"
#include "path.h"
#include "debug.h"
#include "fileiter.h"
#include "termas.h"

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

static int do_rmdirr(struct fileiter_file *file, void *data)
{
	int err;
	const char *name = file->absname;

	if (file->is_dir)
		err = rmdir(name);
	else
		err = unlink(name);

	if (err)
		return warn_errno("failed to remove file `%s'", name);

	return 0;
}

int rmdirr(const char *name)
{
	int ret;
	struct fileiter iter;

	fileiter_init(&iter, name, do_rmdirr, NULL, FI_LIST_DIR);
	ret = fileiter_exec(&iter);
	fileiter_destroy(&iter);

	return ret;
}
