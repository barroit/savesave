// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"
#include "strbuf.h"
#include "strlist.h"
#include "termas.h"
#include "debug.h"
#include "path.h"

static int dispatch_lnkfile(const char *abspath, const char *relpath,
			    file_iterator_cb_t cb, void *data)
{
	struct file_iter_src src = {
		.absname = abspath,
		.relname = relpath,
		.fd      = -1,
		.st      = NULL,
	};

	return cb(&src, data);
}

static int dispatch_regfile(const char *absname, const char *relname,
			    file_iterator_cb_t cb, void *data)
{
	int ret;

	int fd = open(absname, O_RDONLY);
	if (fd == -1)
		return warn_errno(ERR_OPEN_FILE, absname);

	struct stat st;
	ret = fstat(fd, &st);
	if (ret) {
		close(fd);
		return warn_errno(ERR_STAT_FILE, absname);
	}

	struct file_iter_src src = {
		.absname = absname,
		.relname = relname,
		.fd      = fd,
		.st      = &st,
	};

	ret = cb(&src, data);
	close(fd);
	return ret;
}

static int dispatch_file(struct file_iter *ctx, struct dirent *ent)
{
	const char *basename = ent->d_name;
	const char *absname, *relname;

	if (is_dir_indicator(basename))
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basename);

	absname = ctx->sb->str;
	relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	switch (ent->d_type) {
	case DT_REG:
		return dispatch_regfile(absname, relname, ctx->cb, ctx->data);
	case DT_DIR:
		strlist_push2(ctx->sl, absname, 32);
		return 0;
	case DT_LNK:
		return dispatch_lnkfile(absname, relname, ctx->cb, ctx->data);
	case DT_UNKNOWN:
		return warn("can’t determine file type for ‘%s’", absname);
	default:
		warn("‘%s’ has unsupported file type ‘%d’, skipped",
		     absname, ent->d_type);
		return 0;
	}
}

int file_iter_do_exec(struct file_iter *ctx)
{
	const char *dirname = ctx->sb->str;
	DIR *dir = opendir(dirname);
	if (!dir)
		return warn_errno("failed to open directory ‘%s’", dirname);

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
