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

static inline int dispatch_lnkfile(struct fileiter_file *file,
				   struct fileiter *ctx)
{
	if (ctx->flags & FI_USE_STAT) {
		int err = lstat(file->absname, file->st);
		if (err)
			goto err_stat_file;
	}

	return ctx->cb(file, ctx->data);

err_stat_file:
	return warn_errno(_("failed to retrieve information for file `%s'"),
			  file->absname);
}

static int dispatch_regfile(struct fileiter_file *file, struct fileiter *ctx)
{
	int ret;

	if (ctx->flags & FI_USE_FD) {
		file->fd = open(file->absname, O_RDONLY);
		if (file->fd == -1)
			goto err_open_file;
	}

	if (ctx->flags & FI_USE_STAT) {
		if (ctx->flags & FI_USE_FD)
			ret = fstat(file->fd, file->st);
		else
			ret = stat(file->absname, file->st);

		if (ret) {
			int errnum = errno;

			close(file->fd);
			errno = errnum;

			goto err_stat_file;
		}
	}

	ret = ctx->cb(file, ctx->data);
	if (ctx->flags & FI_USE_FD)
		close(file->fd);

	return ret;

err_open_file:
	return warn_errno(_("failed to open file `%s'"), file->absname);
err_stat_file:
	return warn_errno(_("failed to retrieve information for file `%s'"),
			  file->absname);
}

static int dispatch_file(struct fileiter *ctx, struct dirent *ent)
{
	const char *basname = ent->d_name;
	if (is_dir_indicator(basname))
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basname);

	const char *absname = ctx->sb->str;
	const char *relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	switch (ent->d_type) {
	case DT_REG:
		break;
	case DT_DIR:
		strlist_push2(ctx->sl, absname, 32);
		return 0;
	case DT_LNK:
		break;
	case DT_UNKNOWN:
		return warn(_("can't determine file type for `%s'"), absname);
	default:
		warn(_("`%s' has unsupported file type `%d', skipped"),
		     absname, ent->d_type);
		return 0;
	}

	struct fileiter_file file = {
		.absname = absname,
		.dymname = relname,
		.basname = basname,
		.st      = &(struct stat){ 0 },
		.is_lnk  = ent->d_type == DT_LNK,
		.fd      = -1,
	};

	switch (ent->d_type) {
	case DT_REG:
		return dispatch_regfile(&file, ctx);
	case DT_LNK:
		return dispatch_lnkfile(&file, ctx);
	}

	BUG_ON(1);
}

int PLATSPECOF(fileiter_do_exec)(struct fileiter *ctx)
{
	DIR *dir = opendir(ctx->sb->str);
	if (!dir)
		return warn_errno(_("failed to open directory `%s'"),
				  ctx->sb->str);

	int ret;
	struct dirent *ent;

	errno = 0;
	while ((ent = readdir(dir)) != NULL) {
		ret = dispatch_file(ctx, ent);
		if (ret)
			break;

		strbuf_reset(ctx->sb);
	}

	/* A fucking strict check */
	BUG_ON(!ent && errno != 0);

	closedir(dir);
	return ret;
}
