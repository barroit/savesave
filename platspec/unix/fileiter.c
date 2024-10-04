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
#include "path.h"

static inline int dispatch_lnkfile(struct iterfile *file, struct fileiter *ctx)
{
	if (ctx->flag & FITER_USE_STAT) {
		int err = lstat(file->absname, file->st);
		if (err)
			return warn_errno(ERRMAS_STAT_FILE(file->absname));
	}

	return ctx->cb(file, ctx->data);
}

static int dispatch_regfile(struct iterfile *file, struct fileiter *ctx)
{
	int ret;

	if (ctx->flag & FITER_USE_FD) {
		file->fd = open(file->absname, O_RDONLY);
		if (file->fd == -1)
			return warn_errno(ERRMAS_OPEN_FILE(file->absname));
	}

	if (ctx->flag & FITER_USE_STAT) {
		if (ctx->flag & FITER_USE_FD)
			ret = fstat(file->fd, file->st);
		else
			ret = stat(file->absname, file->st);

		if (ret) {
			int errnum = errno;

			close(file->fd);
			errno = errnum;

			return warn_errno(ERRMAS_STAT_FILE(file->absname));
		}
	}

	ret = ctx->cb(file, ctx->data);
	if (ctx->flag & FITER_USE_FD)
		close(file->fd);

	return ret;
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
		if (ctx->flag & FITER_LIST_UNSUP)
			break;
		warn(_("`%s' has unsupported file type `%d', skipped"),
		     absname, ent->d_type);
		return 0;
	}

	struct iterfile file = {
		.absname = absname,
		.dymname = relname,
		.basname = basname,
		.st      = &(struct stat){ 0 },
		.is_lnk  = ent->d_type == DT_LNK,
		.fd      = -1,
	};

	if (ent->d_type == DT_LNK)
		return dispatch_lnkfile(&file, ctx);

	return dispatch_regfile(&file, ctx);
}

int PLATSPECOF(fileiter_loop_dir)(struct fileiter *ctx)
{
	DIR *dir = opendir(ctx->sb->str);
	if (!dir)
		return warn_errno(ERRMAS_OPEN_FILE(ctx->sb->str));

	int ret;
	struct dirent *ent;

	while ((ent = readdir(dir)) != NULL) {
		ret = dispatch_file(ctx, ent);
		if (ret)
			break;

		strbuf_reset(ctx->sb);
	}

	closedir(dir);
	return ret;
}
