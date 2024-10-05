// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"
#include "alloc.h"
#include "strbuf.h"
#include "strlist.h"

static void check_flag(flag_t flag)
{
	BUG_ON(flag & FITER_LIST_DIR && (flag & FITER_RECUR_DIR));
}

void fileiter_init(struct fileiter *ctx,
		   fileiter_function_t cb, void *data, flag_t flag)
{
	DEBUG_RUN()
		check_flag(flag);

	memset(ctx, 0, sizeof(*ctx));

	ctx->sb = xmalloc(sizeof(*ctx->sb));
	strbuf_init(ctx->sb, 0);

	ctx->sl = xmalloc(sizeof(*ctx->sl));
	strlist_init(ctx->sl, 0);

	ctx->cb = cb;
	ctx->data = data;

	ctx->flag = flag;
}

void fileiter_destroy(struct fileiter *ctx)
{
	strbuf_destroy(ctx->sb);
	strlist_destroy(ctx->sl);

	free(ctx->sb);
	free(ctx->sl);
}

static int dispatch_directory(struct fileiter *ctx)
{
	struct iterfile file = {
		.absname = ctx->sb->str,
		.dymname = straftr(file.absname, ctx->root),
		.is_dir  = 1,
		.fd      = -1,
	};

	return ctx->cb(&file, ctx->data);
}

int fileiter_exec(struct fileiter *ctx, const char *root)
{
	int err;
	const char *dir = root;
	size_t rootlen = strlen(root);
	size_t baslen = strlen(dir);
	size_t ndrlen;

	ctx->root = root;
	while (39) {
		strbuf_reset_from(ctx->sb, dir);

		if (ctx->flag & FITER_LIST_DIR) {
			err = dispatch_directory(ctx);
			if (unlikely(err))
				return -1;
		}

		err = PLATSPECOF(fileiter_loop_dir)(ctx);
		if (unlikely(err))
			return -1;

		dir = strlist_pop2(ctx->sl, 0);
		if (unlikely(!dir)) {
			if (!(ctx->flag & FITER_RECUR_DIR))
				return 0;

			strbuf_reset(ctx->sb);
			ndrlen = baslen;
			goto iter_done;
		}

		if (!(ctx->flag & FITER_RECUR_DIR))
			continue;
		strbuf_reset(ctx->sb);

		const char *prev = ctx->sb->str, *next = dir;
		ndrlen = strrchr(next, '/') - next;

		/* if previous name is the prefix of next name, we skip it */
		if (strlen(prev) == ndrlen)
			continue;
		size_t pdrlen = strrchr(prev, '/') - prev;

		/* previous name has the same parent as next name */
		if (pdrlen == ndrlen) {
			err = dispatch_directory(ctx);
			if (unlikely(err))
				return -1;
			continue;
		}

		/* previous branch is done */
		while (39) {
iter_done:
			err = dispatch_directory(ctx);
			if (unlikely(err))
				return -1;

			if (unlikely(ctx->sb->len == rootlen))
				return 0;

			strbuf_to_dirname(ctx->sb);
			if (ctx->sb->len == ndrlen) {
				if (likely(dir))
					break;
				else
					return dispatch_directory(ctx);
			}
		}
	}
}
