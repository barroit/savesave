// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "termsg.h"

int is_abs_path(const char *path)
{
	return *path == '/';
}

int copy_file(const char *src, int fd1, struct stat *st, const char *dest)
{
	int fd2 = creat(dest, 0664);
	if (fd2 == -1)
		return error_errno("failed to create file at ‘%s’", dest);

	int ret = 0;
	off_t remain = st->st_size;
	ssize_t copied;
	do {
		copied = copy_file_range(fd1, NULL, fd2, NULL, remain, 0);
		if (copied == -1) {
			error_errno("failed to copy file from ‘%s’ to ‘%s’",
				    src, dest);
			ret = -1;
			break;
		}

		remain -= copied;
	} while (remain > 0 && copied > 0);

	close(fd2);
	return ret;
}
