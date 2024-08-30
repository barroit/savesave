// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termsg.h"
#include "strlist.h"
#include "alloc.h"
#include "debug.h"
#include "strbuf.h"
#include "robio.h"

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

void file_iter_init(struct file_iter *ctx, const char *head,
			file_iterator_cb_t cb, void *data)
{
	memset(ctx, 0, sizeof(*ctx));

	ctx->head = head;

	ctx->sb = xmalloc(sizeof(*ctx->sb));
	strbuf_init(ctx->sb, 0);

	ctx->sl = xmalloc(sizeof(*ctx->sl));
	strlist_init(ctx->sl, STRLIST_DUPSTR);

	ctx->cb = cb;
	ctx->data = data;
}

void file_iter_destroy(struct file_iter *ctx)
{
	strbuf_destroy(ctx->sb);
	strlist_destroy(ctx->sl);

	free(ctx->sb);
	free(ctx->sl);
}

static int dispatch_file(struct file_iter *ctx, struct dirent *ent)
{
	int ret;
	int fd;
	struct stat *st = &(struct stat){ 0 };
	const char *basename = ent->d_name;
	const char *pathname;

	if (strcmp(basename, "..") == 0 ||
	    strcmp(basename, ".") == 0)
		return 0;

	if (strcmp(ctx->sb->str, "/") != 0)
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basename);
	pathname = ctx->sb->str;

	switch (ent->d_type) {
	case DT_REG:
		fd = open(pathname, O_RDONLY);
		if (fd == -1)
			goto err_open_file;

		ret = fstat(fd, st);
		if (ret)
			goto err_stat_file;

		break;
	case DT_DIR:
		strlist_push2(ctx->sl, pathname, 32);
		return 0;
	case DT_LNK:
		fd = -1;
		st = NULL;
		break;
	case DT_UNKNOWN:
		return warn("can’t determine file type for ‘%s’", pathname);
	default:
		warn("‘%s’ has unsupported file type ‘%d’, skipped",
		     pathname, ent->d_type);
		return 0;
	}

	ret = ctx->cb(pathname, fd, st, ctx->data);
	close(fd);

	return ret;

err_stat_file:
	close(fd);
	return warn_errno("failed to retrieve information for file ‘%s’",
			  pathname);
err_open_file:
	return warn_errno("failed to open file ‘%s’", pathname);
}

static int do_iterate(struct file_iter *ctx)
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

int file_iter_exec(struct file_iter *ctx)
{
	int ret;
	const char *dir = ctx->head;

	do {
		strbuf_concat(ctx->sb, dir);

		ret = do_iterate(ctx);
		if (ret)
			return 1;

		strbuf_zerolen(ctx->sb);
	} while ((dir = strlist_pop2(ctx->sl, 0)));

	return 0;
}
