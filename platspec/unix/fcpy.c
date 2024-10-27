// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fcpy.h"
#include "termas.h"
#include "alloc.h"

int copy_regfile(const char *srcname, const char *destname)
{
	int ret = -1;
	int src = open(srcname, O_RDONLY);
	int dest = -1;
	struct stat st;

	if (src == -1)
		goto err_open_src;

	ret = fstat(src, &st);
	if (ret)
		goto err_stat_src;

	dest = flexcreat(destname);
	if (dest == -1)
		goto err_creat_dest;

	if (!st.st_size)
		goto done;

	ret = FEATSPEC(fd2fd_copy)(src, dest, st.st_size);
	if (ret)
		goto err_copy_file;

	if (0) {
	err_open_src:
		warn_errno(ERRMAS_OPEN_FILE(srcname));
	} else if (0) {
	err_stat_src:
		warn_errno(ERRMAS_STAT_FILE(srcname));
	} else if (0) {
	err_creat_dest:
		warn_errno(ERRMAS_CREAT_FILE(destname));
	} else if (0) {
	err_copy_file:
		warn_errno(ERRMAS_COPY_FILE(srcname, destname));

		close(dest);
		unlink(destname);
		dest = -1;
	}

done:
	if (src != -1)
		close(src);
	if (dest != -1)
		close(dest);

	return ret;
}

int copy_symlink(const char *srcname, const char *destname)
{
	int ret;
	struct stat st;

	ret = lstat(srcname, &st);
	if (ret)
		return warn_errno(ERRMAS_STAT_FILE(srcname));

	size_t len = st.st_size ? st.st_size + 1 : PATH_MAX;
	char *buf = xmalloc(len);
	ssize_t nr = readlink(srcname, buf, len);

	if (nr == -1)
		goto err_read_link;

	if (nr == len) {
		warn(_("path `%s' may have been truncated"), buf);
		nr -= 1;
	}
	buf[nr] = 0;

	ret = symlink(buf, destname);
	if (ret)
		goto err_creat_link;

	if (0) {
	err_read_link:
		ret = warn_errno(ERRMAS_READ_LINK(srcname));
	} else if (0) {
	err_creat_link:
		ret = warn_errno(ERRMAS_CREAT_LINK(destname, srcname));
	}

	free(buf);
	return ret;
}
