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
#include "win/termas.hpp"
#include "debug.h"
#include "path.h"

extern "C"{
int fileiter_do_exec(struct fileiter *ctx);
}

static int dispatch_file(struct fileiter *ctx, WIN32_FIND_DATA *ent)
{
	const char *basname = ent->cFileName;
	const char *absname, *relname;

	if (is_dir_indicator(basname))
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basname);

	absname = ctx->sb->str;
	relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	struct fileiter_file src = {
		.absname = absname,
		.dymname = relname,
		.fd      = -1,
		.st      = NULL,
	};

	/*
	 * A reparse point could also have a FILE_ATTRIBUTE_DIRECTORY flag,
	 * we need to figure out whether it is a symlink FIRST since we don't
	 * want to open/traverse files/directories pointed by a symlink
	 */
	if (ent->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
		return ctx->cb(&src, ctx->data);
	} else if (ent->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		strlist_push2(ctx->sl, absname, 32);
		return 0;
	}

	int ret;
	struct stat st;

	ret = stat(absname, &st);
	if (ret)
		return warn_errno(_("failed to retrieve information for file `%s'"),
				  absname);

	src.st = &st;
	if (S_ISREG(st.st_mode)) {
		int fd = open(absname, O_RDONLY);
		if (fd == -1)
			return warn_errno(_("failed to open file `%s'"),
					  absname);

		src.fd = fd;
		ret = ctx->cb(&src, ctx->data);
		close(fd);
		return ret;
	} else {
		warn(_("`%s' has unsupported st_mode `%ud', skipped"),
		     absname, st.st_mode);
		return 0;
	}
}

int fileiter_do_exec(struct fileiter *ctx)
{
	strbuf_concat(ctx->sb, "/*");

	int ret = 0;
	WIN32_FIND_DATA ent;

	const char *pattern = ctx->sb->str;
	HANDLE dir = FindFirstFile(pattern, &ent);

	if (dir == INVALID_HANDLE_VALUE)
		goto err_find_file;

	do {
		ret = dispatch_file(ctx, &ent);
		if (ret)
			break;

		strbuf_reset(ctx->sb);
	} while (FindNextFile(dir, &ent) != NULL);

	FindClose(dir);
	return ret;

err_find_file:
	return warn_winerr(_("failed to find first file from pattern `%s'"),
			   pattern);
}
