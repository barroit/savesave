// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * we tried cpp17's file system, but the api is sucked, especially when coming
 * with temporary object and value_type. So, use win32 api.
 */

#include "fileiter.h"
#include "strbuf.h"
#include "strlist.h"
#include "termas.h"
#include "debug.h"
#include "path.h"

static int dispatch_file(struct fileiter *ctx, WIN32_FIND_DATA *ent)
{
	const char *basname = ent->cFileName;
	if (is_dir_indicator(basname))
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basname);

	const char *absname = ctx->sb->str;
	const char *relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	struct fileiter_file file = {
		.absname = absname,
		.dymname = relname,
		.basname = basname,
		.st      = &(struct stat){ 0 },
		.is_lnk  = 0,
		.fd      = -1,
	};

	/*
	 * A reparse point could also have a FILE_ATTRIBUTE_DIRECTORY flag,
	 * we need to figure out whether it is a symlink FIRST since we don't
	 * want to open/traverse files/directories pointed by a symlink
	 */
	if (ent->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
		file.is_lnk = 1;
		return ctx->cb(&file, ctx->data);
	} else if (ent->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		strlist_push2(ctx->sl, absname, 32);
		return 0;
	}

	if (ctx->flags & FI_USE_STAT) {
		int err = stat(absname, file.st);
		if (err)
			goto err_stat_file;
	}

	if (S_ISREG(file.st->st_mode))
		return ctx->cb(&file, ctx->data);

	warn(_("`%s' has unsupported st_mode `%ud', skipped"),
	     absname, file.st->st_mode);
	return 0;

err_stat_file:
	return warn_errno(_("failed to retrieve information for file `%s'"),
			  absname);
}

int PLATSPECOF(fileiter_do_exec)(struct fileiter *ctx)
{
	strbuf_concat(ctx->sb, "/*");

	int ret = 0;
	WIN32_FIND_DATA ent;
	HANDLE dir = FindFirstFile(ctx->sb->str, &ent);

	if (dir == INVALID_HANDLE_VALUE)
		goto err_find_file;

	SetLastError(0);
	do {
		strbuf_reset(ctx->sb);
		ret = dispatch_file(ctx, &ent);
		if (ret)
			break;

		BUG_ON(GetLastError());
	} while (FindNextFile(dir, &ent));

	if (GetLastError() != ERROR_NO_MORE_FILES)
		warn_winerr(_("failed to find next file of file `%s'"),
			    ctx->sb->str);
	else
		SetLastError(0);

	FindClose(dir);
	return ret;

err_find_file:
	return warn_winerr(_("failed to find first file from pattern `%s'"),
			   ctx->sb->str);
}
