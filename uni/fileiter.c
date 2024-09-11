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

static int dispatch_lnkfile(const char *absname, const char *relname,
			    const char *basname, fileiter_callback cb,
			    void *data)
{
	struct fileiter_file file = {
		.absname = absname,
		.dymname = relname,
		.basname = basname,
		.fd      = -1,
		.st      = NULL,
	};

	return cb(&file, data);
}

static int dispatch_regfile(const char *absname, const char *relname,
			    const char *basname, fileiter_callback cb,
			    void *data)
{
	int ret;

	int fd = open(absname, O_RDONLY);
	if (fd == -1)
		return warn_errno(_("failed to open file `%s'"), absname);

	struct stat st;
	ret = fstat(fd, &st);
	if (ret) {
		close(fd);
		return warn_errno(_("failed to retrieve information for file `%s'"),
				  absname);
	}

	struct fileiter_file file = {
		.absname = absname,
		.dymname = relname,
		.basname = basname,
		.fd      = fd,
		.st      = &st,
	};

	ret = cb(&file, data);
	close(fd);
	return ret;
}

static int dispatch_file(struct fileiter *ctx, struct dirent *ent)
{
	const char *basname = ent->d_name;
	const char *absname, *relname;

	if (is_dir_indicator(basname))
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basname);

	absname = ctx->sb->str;
	relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	switch (ent->d_type) {
	case DT_REG:
		return dispatch_regfile(absname, relname, basname,
					ctx->cb, ctx->data);
	case DT_DIR:
		strlist_push2(ctx->sl, absname, 32);
		return 0;
	case DT_LNK:
		return dispatch_lnkfile(absname, relname, basname,
					ctx->cb, ctx->data);
	case DT_UNKNOWN:
		return warn(_("can't determine file type for `%s'"), absname);
	default:
		warn(_("`%s' has unsupported file type `%d', skipped"),
		     absname, ent->d_type);
		return 0;
	}
}

int fileiter_do_exec(struct fileiter *ctx)
{
	const char *dname = ctx->sb->str;
	DIR *dir = opendir(dname);
	if (!dir)
		return warn_errno(_("failed to open directory `%s'"), dname);

	int ret;
	int errnum;
	struct dirent *ent;

	errno = 0;
	while ((ent = readdir(dir)) != NULL) {
		ret = dispatch_file(ctx, ent);
		if (ret) {
			errnum = errno;
			break;
		}

		strbuf_reset(ctx->sb);
	}

	closedir(dir);

	errno = errnum;
	return ret;
}
