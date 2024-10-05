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
#include "path.h"

static int dispatch_file(struct __fileiter *ctx, WIN32_FIND_DATA *ent)
{
	/* this sucking dwFileAttributes design makes condition check mess */
	if (ctx->flag & FITER_DIR_ONLY &&
	    !(ent->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
	    !(ent->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
		return 0;

	const char *basname = ent->cFileName;
	if (is_dir_indicator(basname))
		return 0;

	if (ctx->sb.str[ctx->sb.len - 1] != '/')
		strbuf_concat(&ctx->sb, "/");
	strbuf_concat(&ctx->sb, basname);

	const char *absname = ctx->sb.str;
	const char *relname = straftr(absname, ctx->root);
	BUG_ON(!relname);

	struct iterfile file = {
		.absname = absname,
		.dymname = relname,
		.basname = basname,
		.fd      = -1,
	};

	/*
	 * A reparse point could also have a FILE_ATTRIBUTE_DIRECTORY flag,
	 * we need to figure out whether it is a symlink FIRST since we don't
	 * want to open/traverse files/directories pointed by a symlink
	 */
	if (ent->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
		file.st.st_mode = S_IFLNK;
		return ctx->func(&file, ctx->data);
	} else if (ent->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		strlist_push2(&ctx->sl, absname, 32);
		return 0;
	}

	if (ctx->flag & FITER_USE_STAT) {
		int err = stat(absname, &file.st);
		if (unlikely(err)) {
			warn_errno(ERRMAS_STAT_FILE(absname));
			if (errno != ENOENT)
				return -1;
			return warn(_("a broken symlink?"));
		}
	}

	if (S_ISREG(file.st.st_mode))
		return ctx->func(&file, ctx->data);

	if (ctx->flag & FITER_LIST_UNSUP ||
	    (ent->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)) {
		file.st.st_mode = S_IFREG;
		return ctx->func(&file, ctx->data);
	}

	warn(_("`%s' has unsupported st_mode `%u', skipped"),
	     absname, file.st.st_mode);
	return 0;
}

int PLATSPECOF(fileiter_loop_dir)(struct __fileiter *ctx)
{
	strbuf_concat(&ctx->sb, "/*");

	int ret = 0;
	WIN32_FIND_DATA ent;
	HANDLE dir = FindFirstFile(ctx->sb.str, &ent);

	if (dir == INVALID_HANDLE_VALUE)
		goto err_find_file;

	do {
		strbuf_reset(&ctx->sb);
		ret = dispatch_file(ctx, &ent);
		if (ret) {
			SetLastError(ERROR_NO_MORE_FILES);
			break;
		}
	} while (FindNextFile(dir, &ent));

	if (GetLastError() != ERROR_NO_MORE_FILES)
		warn_winerr(_("failed to find next file of file `%s'"),
			    ctx->sb.str);

	FindClose(dir);
	return ret;

err_find_file:
	return warn_winerr(_("failed to find first file from pattern `%s'"),
			   ctx->sb.str);
}
