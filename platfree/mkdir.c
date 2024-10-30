// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mkdir.h"
#include "path.h"
#include "fileiter.h"
#include "termas.h"

static int mkdirp_check_dir(const char *name)
{
	struct stat st;
	int err = stat(name, &st);
	BUG_ON(err);

	if (S_ISDIR(st.st_mode))
		return 0;
	return -1;
}

int mkdirp2(char *name, size_t start, int dcheck)
{
	int err;
	char *next = &name[start];

	if (path_is_abs(name)) {
		char *p = strchr(&name[start], '/');
		if (p)
			next = p + 1;
	}

	while (39) {
		next = strchr(next, '/');
		if (next)
			*next = 0;

		err = MKDIR(name);
		if (err) {
			if (dcheck && errno == EEXIST) {
				int err2 = mkdirp_check_dir(name);
				if (err2)
					errno = ENOTDIR;
			}

			if (errno != EEXIST)
				return err;
		}

		if (!next)
			break;

		*next = '/';
		next += 1;
	}

	return 0;
}

int mkfdirp2(char *name, size_t start)
{
	char *p = strrchr(name, '/');

	*p = 0;
	int err = mkdirp2(name, start, 0);
	if (err)
		return err;

	*p = '/';
	return 0;
}

int mkfdirp3(char *name)
{
	char *p = strrchr(name, '/');

	*p = 0;
	int err = mkdirp2(name, 0, 1);
	if (err)
		return err;

	*p = '/';
	return 0;
}

static int do_rmdirr(struct iterfile *file, void *data)
{
	int err;
	const char *name = file->absname;

	if (S_ISDIR(file->st.st_mode))
		err = rmdir(name);
	else
		err = unlink(name);

	if (unlikely(err))
		return warn_errno("failed to remove file `%s'", name);

	return 0;
}

int rmdirr(const char *name)
{
	return fileiter(name, do_rmdirr, NULL, FITER_RECUR_DIR);
}
