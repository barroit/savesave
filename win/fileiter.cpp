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
int file_iter_do_exec(struct file_iter *ctx);
}

static int dispatch_file(struct file_iter *ctx, WIN32_FIND_DATA *ent)
{
	const char *basename = ent->cFileName;
	const char *absname, *relname;

	if (is_dir_indicator(basename))
		return 0;

	if (ctx->sb->str[ctx->sb->len - 1] != '/')
		strbuf_concat(ctx->sb, "/");
	strbuf_concat(ctx->sb, basename);

	absname = ctx->sb->str;
	relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	struct file_iter_src src = {
		.absname = absname,
		.relname = relname,
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
		return warn_errno(ERR_STAT_FILE, absname);

	src.st = &st;
	if (S_ISREG(st.st_mode)) {
		int fd = open(absname, O_RDONLY);
		if (fd == -1)
			return warn_errno(ERR_OPEN_FILE, absname);

		src.fd = fd;
		ret = ctx->cb(&src, ctx->data);
		close(fd);
		return ret;
	} else {
		warn("‘%s’ has unsupported st_mode ‘%ud’, skipped",
		     absname, st.st_mode);
		return 0;
	}
}

int file_iter_do_exec(struct file_iter *ctx)
{
	int ret = 0;
	WIN32_FIND_DATA ent;
	const char *dirname = ctx->sb->str;
	size_t dirlen = ctx->sb->len;

	strbuf_concat(ctx->sb, "/*");
	HANDLE dir = FindFirstFile(ctx->sb->str, &ent);
	if (dir == INVALID_HANDLE_VALUE)
		return warn_winerr("failed to open directory ‘%s’", dirname);

	do {
		ret = dispatch_file(ctx, &ent);
		if (ret)
			break;

		strbuf_truncate(ctx->sb, ctx->sb->len - dirlen);
	} while (FindNextFile(dir, &ent) != NULL);

	FindClose(dir);
	return ret;
}
