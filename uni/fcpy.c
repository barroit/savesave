// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fcpy.h"
#include "termas.h"
#include "alloc.h"

static int copy(int src, int dest, off_t len)
{
	ssize_t copied;
	do {
		copied = copy_file_range(src, NULL, dest, NULL, len, 0);
		if (copied == -1)
			return -1;

		len -= copied;
	} while (len > 0 && copied > 0);

	return 0;
}

int copy_regfile(const char *srcname, const char *destname)
{
	int ret;
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

	ret = copy(src, dest, st.st_size);
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


int copy_regfile_async(const char *srcname, const char *destname)
{
	return 0;
}
