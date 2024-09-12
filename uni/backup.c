// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"
#include "strbuf.h"
#include "uni/cp.h"
#include "termas.h"
#include "debug.h"
#include "path.h"

int backup_copy_regfile(struct fileiter_file *src,
			struct strbuf *dest, struct strbuf *_)
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

static int readlink3(const char *name, struct strbuf *__buf)
{
	int err;
	struct stat st;

	err = lstat(name, &st);
	if (err)
		return -1;

	size_t len = st.st_size ? st.st_size + 1 : PATH_MAX;
	strbuf_require_cap(__buf, len);

	ssize_t nr = readlink(name, __buf->str, len);
	if (nr == -1)
		return -1;

	__buf->str[nr] = 0;
	__buf->len = nr;
	if (nr == len)
		warn(_("path `%s' may have been truncated"), __buf->str);

	return 0;
}

int backup_copy_symlink(const char *src,
			struct strbuf *dest, struct strbuf *__buf)
{
	int ret;

	ret = readlink3(src, __buf);
	if (ret)
		goto err_read_link;

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
	return warn_errno(_("unable to real link `%s'"), src);
err_make_link:
	BUG_ON(errno == ENOENT);
	return warn_errno(_("unable to make symbolic link `%s'"), src);
err_make_dir:
	return warn_errno(_("unable to make directory `%s'"), dest->str);
}
