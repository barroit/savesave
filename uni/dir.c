// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "robio.h"
#include "strbuf.h"
#include "strlist.h"
#include "termsg.h"
#include "debug.h"

static char *get_user_home(const char *user)
{
	struct passwd *pw = getpwnam(user);
	if (!pw) {
		warn_errno("can’t get home for ‘%s’ who invoked sudo", user);
		return NULL;
	}

	return pw->pw_dir;
}

const char *get_home_dir(void)
{
	const char *home;
	const char *user = getenv("SUDO_USER");
	if (user)
		home = get_user_home(user);
	else
		home = getenv("HOME");
	if (!home)
		die("your $HOME corrupted");
	return home;
}

static int dispatch_regfile(const char *name,
			    file_iterator_cb_t cb, void *data)
{
	int ret;

	int fd = open(name, O_RDONLY);
	if (fd == -1)
		goto err_open_file;

	struct stat st;
	ret = fstat(fd, &st);
	if (ret)
		goto err_stat_file;

	ret = cb(name, fd, &st, ctx->data);
	close(fd);
	return ret;

err_stat_file:
	close(fd);
	return warn_errno("failed to retrieve information for file ‘%s’",
			  pathname);
err_open_file:
	return warn_errno("failed to open file ‘%s’", pathname);
}

static int dispatch_file(struct file_iter *ctx, struct dirent *ent)
{
	int ret;
	const char *basename = ent->d_name;
	const char *pathname;

	if (strcmp(basename, "..") == 0 ||
	    strcmp(basename, ".") == 0)
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basename);
	pathname = ctx->sb->str;

	switch (ent->d_type) {
	case DT_REG:
		return dispatch_regfile(pathname, ctx->cb, ctx->data);
	case DT_DIR:
		strlist_push2(ctx->sl, pathname, 32);
		return 0;
	case DT_LNK:
		return ctx->cb(name, -1, NULL, ctx->data);
	case DT_UNKNOWN:
		return warn("can’t determine file type for ‘%s’", pathname);
	default:
		warn("‘%s’ has unsupported file type ‘%d’, skipped",
		     pathname, ent->d_type);
		return 0;
	}
}

int file_iter_do_exec(struct file_iter *ctx)
{
	const char *dirname = ctx->sb->str;
	DIR *dir = opendir(dirname);
	if (!dir)
		return warn_errno("failed to open dir ‘%s’", dirname);

	int ret;
	struct dirent *ent;
	size_t dirlen = ctx->sb->len;

	errno = 0;
	while ((ent = readdir(dir)) != NULL) {
		ret = dispatch_file(ctx, ent);
		if (ret)
			break;

		strbuf_truncate(ctx->sb, ctx->sb->len - dirlen);
	}

	closedir(dir);
	return ret;
}
