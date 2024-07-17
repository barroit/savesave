// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include <fcntl.h>
#include <sys/stat.h>
#include "barroit/io.h"
#include "alloc.h"
#include <stdlib.h>

#ifdef WINDOWS_NATIVE
# define open  _open
# define fstat _fstat
# define close _close
# define stat  _stat64i32
#endif

char *readfile(const char *name)
{
	int fd = open(name, O_RDONLY);
	if (fd == -1)
		goto err_open_file;

	struct stat st;
	if (fstat(fd, &st) == -1)
		goto err_stat_df;

	char *buf = xmalloc(st.st_size + 1);
	buf[st.st_size] = 0;

	if (robread(fd, buf, st.st_size) == -1)
		goto err_read_file;

	close(fd);
	return buf;

err_read_file:
	free(buf);
err_stat_df:
	close(fd);
err_open_file:
	return NULL;
}
