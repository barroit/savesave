// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "barroit/io.h"
#include "alloc.h"

char *readfile(const char *name)
{
	int fd = open(name, O_RDONLY);
	if (fd == -1)
		goto err_open_file;

	struct stat st;
	if (fstat(fd, &st) == -1)
		goto err_stat_fd;

	char *buf = xmalloc(st.st_size + 1);
	buf[st.st_size] = 0;

	if (robread(fd, buf, st.st_size) == -1)
		goto err_read_file;

	close(fd);
	return buf;

err_read_file:
	free(buf);
err_stat_fd:
	close(fd);
err_open_file:
	return NULL;
}
