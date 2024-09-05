// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "ioop.h"
#include "alloc.h"
#include "termas.h"

int redirect_stdio(const char *name)
{
	int err;

	char *tmp = xstrdup(name);
	const char *dir = dirname(tmp);

	err = my_mkdir(dir);
	free(tmp);

	if (err && errno != EEXIST)
		return error_errno("failed to create directory for log file ‘%s’",
				   name);

	int fd = creat(name, 0664);
	if (fd == -1) {
		error_errno(ERR_CREAT_FILE, name);
		goto err_open_file;
	}

	if (dup2(fd, fileno(stdout)) == -1) {
		error_errno("cannot redirect stdout to ‘%s’", name);
		goto close_file;
	}

	if (dup2(fd, fileno(stderr)) == -1) {
		error_errno("cannot redirect stderr to ‘%s’", name);
		goto close_file;
	}

	close(fd);
	return 0;

close_file:
	close(fd);
err_open_file:
	return 1;
}
