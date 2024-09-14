// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"
#include "strbuf.h"
#include "termas.h"
#include "debug.h"
#include "path.h"
#include "robio.h"

// static char buf[SZ_512K];

// int copyfile(int src, int dest, off_t len)
// {
// 	ssize_t nr, nw;
// 	do {
// 		nr = robread_all(src, buf, sizeof(buf));
// 		if (unlikely(nr == -1))
// 			return -1;

// 		nw = robwrite_all(dest, buf, nr);
// 		if (unlikely(nw == -1))
// 			return -1;
// 	} while (nr > 0);

// 	return 0;
// }

static int copyfile(int src, int dest, off_t len)
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

int PLATSPECOF(backup_copy_regfile)(struct fileiter_file *src,
				    struct strbuf *dest)
{
	int ret;
	int destfd = creat(dest->str, 0664);

	if (destfd == -1) {
		if (errno != ENOENT)
			goto err_create_file;

		ret = strbuf_mkfdirp(dest);
		if (ret)
			goto err_make_dir;

		destfd = creat(dest->str, 0664);
		if (destfd == -1)
			goto err_create_file;
	}

	ret = copyfile(src->fd, destfd, src->st->st_size);
	if (ret)
		warn_errno(_("unable to copy file from `%s' to `%s'"),
			   src->absname, dest->str);

	close(destfd);
	return ret;

err_create_file:
	BUG_ON(errno == ENOENT);
	return warn_errno(_("unable to create file at `%s'"), dest->str);
err_make_dir:
	return warn_errno(_("unable to make directory `%s'"), dest->str);
}

int PLATSPECOF(backup_copy_symlink)(struct fileiter_file *src,
				    struct strbuf *dest, struct strbuf *__buf)
{
	int ret;
	size_t len = src->st->st_size ? src->st->st_size + 1 : PATH_MAX;

	strbuf_require_cap(__buf, len);
	ssize_t nr = readlink(src->absname, __buf->str, len);
	if (nr == -1)
		goto err_read_link;

	__buf->str[nr] = 0;
	__buf->len = nr;
	if (nr == len)
		warn(_("path `%s' may have been truncated"), __buf->str);

	ret = symlink(__buf->str, dest->str);
	if (ret) {
		if (errno != ENOENT)
			goto err_make_link;

		ret = strbuf_mkfdirp(dest);
		if (ret)
			goto err_make_dir;

		ret = symlink(__buf->str, dest->str);
		if (ret)
			goto err_make_link;
	}

	return 0;

err_read_link:
	return warn_errno(_("unable to real link `%s'"), src->absname);
err_make_link:
	BUG_ON(errno == ENOENT);
	return warn_errno(_("unable to make symbolic link `%s'"),
			  src->absname);
err_make_dir:
	return warn_errno(_("unable to make directory `%s'"), dest->str);
}
